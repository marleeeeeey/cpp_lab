#include <../../GameShared/include/GameShared/GameMessageTypes.h>
#include <App.h>
#include <GameSerialization/GameSerialization.h>
#include <spdlog/spdlog.h>

#include <array>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <random>
#include <string>
#include <string_view>

#include "CommandLineParser.h"
#include "GameLoop.h"
#include "GetClientIpText.h"
#include "HumanHash.h"
#include "NetworkDataHandler/INetworkDataHandler.h"
#include "PerSocketData.h"
#include "ServerState.h"
#include "WsCloseCodeToText.h"

int main(int argc, char** argv) {  // Uncomment the next line for Debug
  // spdlog::set_level(spdlog::level::trace);

  SPDLOG_INFO("");
  SPDLOG_INFO("---------------------------");
  SPDLOG_INFO("GameServer starting...     ");
  SPDLOG_INFO("---------------------------");
  SPDLOG_INFO("");

  CommandLineParser commandLineParser(argc, argv);
  int port = commandLineParser.getPort();
  auto state = std::make_shared<ServerState>();

  state->gameSession = std::make_unique<GameSession>();

  // --------------------------------------
  // Create BinaryMessageParser instance
  // --------------------------------------

  // TODO create wrapper around uWS::App and make this function as interface of this wrapper
  auto sendTypedBinaryToWebSocket = [&state](GameMessageType type, WsType* ws, PayloadView payload) {
    auto typedPayload = state->networkDataHandler->makeBinaryMessage(type, payload);
    std::string_view stringView(reinterpret_cast<const char*>(typedPayload.data()), typedPayload.size());
    ws->send(stringView, uWS::OpCode::BINARY);
  };

  // IMPORTANT: Use the main loop for deferring tasks.
  // Otherwise, "defer" callback will no be executed.
  uWS::Loop* mainLoop = uWS::Loop::get();

  BinaryMessageParser::OnBroadcastMessageCallback onBroadcastMessageCallback =
      [state, mainLoop](const PayloadType type, PayloadView replyPayload) {
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        SPDLOG_TRACE("Broadcasting message {}. ThreadId {}", type, oss.str());
        auto typedPayload = state->networkDataHandler->makeBinaryMessage(type, replyPayload);
        mainLoop->defer([state, typedPayload] {
          SPDLOG_TRACE("Broadcasting message as uWS::OpCode::BINARY");
          std::string_view messageStringView(reinterpret_cast<const char*>(typedPayload.data()), typedPayload.size());
          state->app->publish(state->getBroadcastTopicName(), messageStringView, uWS::OpCode::BINARY);  // broadcast
        });
      };

  state->binaryMessageParser = std::make_unique<BinaryMessageParser>(
      onBroadcastMessageCallback, sendTypedBinaryToWebSocket);

  // ---------------------
  // Upgrade (detect IP)
  // ---------------------

  auto onSocketUpgrade = [](uWS::HttpResponse<false>* response, uWS::HttpRequest* request, struct us_socket_context_t* context) {
    // Set callback "onAborted"
    response->onAborted([]() { SPDLOG_WARN("upgrade aborted by client"); });

    // Get IP address hided by nginx
    auto socketIpText = response->getRemoteAddressAsText();
    const auto clientIpRaw = getClientIpText(request, socketIpText);

    // Calculate a human-readable name as hash from client addr
    auto clientAddr = convertKnownIpToShortName(clientIpRaw);
    auto humanName = HumanHash::getShortHumanName(clientIpRaw);
    SPDLOG_DEBUG("ws.upgrade. clientIp={}, setName={}, socketIp={}, clientIpRaw={}, ",
                 clientAddr, humanName, socketIpText, clientIpRaw);

    // Create and move PerSocketData to .open method
    PerSocketData perSocketData;
    perSocketData.player.name = humanName;
    perSocketData.clientIp = clientAddr;
    response->upgrade<PerSocketData>(
        std::move(perSocketData),
        request->getHeader("sec-websocket-key"),         // required
        request->getHeader("sec-websocket-protocol"),    // optional
        request->getHeader("sec-websocket-extensions"),  // optional
        context);
  };

  // ------------------------
  // Open (new connection)
  // ------------------------

  auto onSocketOpen = [state, sendTypedBinaryToWebSocket](auto* ws) {
    PerSocketData* perSocketData = (PerSocketData*)ws->getUserData();
    ws->subscribe(state->getBroadcastTopicName());
    state->incrementNumberOfClients();

    Player& player = perSocketData->player;
    static PlayerId nextPlayerId = 0;
    player.id = nextPlayerId++;
    auto name = player.name;
    player.state.position = {100.0f, 100.0f};
    while (state->connectedClientNames.contains(name)) {
      name = HumanHash::getShortHumanName(perSocketData->clientIp + name);
    }
    player.name = name;
    state->connectedClientNames.insert(name);
    SPDLOG_DEBUG("ws.open");
    SPDLOG_INFO("{} connected from ip {}. Player id: {}",
                player.name, perSocketData->clientIp, player.id);

    // ---------------------------
    // Add user to game session
    // ---------------------------

    {
      std::lock_guard lock(state->gameSession->mutex);
      state->gameSession->perSocketDatas.push_back(perSocketData);
    }

    auto payload = GameSerialization::serializeMemcpy(player.id);
    sendTypedBinaryToWebSocket(GMT_PlayerIdFromServer, ws, payload);
  };

  // -------------------------
  // Close (connection end)
  // -------------------------

  auto onSocketClose = [state](auto* ws, int code, std::string_view msg) {
    PerSocketData* perSocketData = (PerSocketData*)ws->getUserData();
    msg = msg.empty() ? wsCloseCodeToText(code) : msg;
    SPDLOG_DEBUG("ws.close. ip={}, name={}, code={}, msg={}",
                perSocketData->clientIp, perSocketData->player.name, code, msg);

    SPDLOG_INFO("{} disconnected", perSocketData->player.name);
    state->connectedClientNames.erase(perSocketData->player.name);

    // ---------------------------------
    // Remove user from game session
    // ---------------------------------

    {
      std::lock_guard lock(state->gameSession->mutex);
      std::erase(state->gameSession->perSocketDatas, perSocketData);
    }

    state->decrementNumberOfClients(); };

  // ---------------------------
  // Message (incoming data)
  // ---------------------------

  auto onSocketMessage = [state](auto* ws, std::string_view msg, uWS::OpCode op) {
    PerSocketData* perSocketData = (PerSocketData*)ws->getUserData();
    auto& player = perSocketData->player;
    player.messagesSent++; // update message count; just for statistics

    // Text messages are not supported
    if (op == uWS::OpCode::TEXT) {
      SPDLOG_CRITICAL("ws.message. From {}; op={}", perSocketData->player.name, magic_enum::enum_name(op));
    }

    // Detect a binary message type and delegate parsing to BinaryMessageParser
    if (op == uWS::OpCode::BINARY) {
      std::vector<uint8_t> binaryMsg = std::vector<uint8_t>(msg.begin(), msg.end());  // TODO: remove unnecessary copy
      SPDLOG_DEBUG("ws.message. From {}; op={}", perSocketData->player.name, magic_enum::enum_name(op));
      state->networkDataHandler->parseBinaryMessage(
        binaryMsg,
          [&](const PayloadType type, PayloadView payload) {
            state->binaryMessageParser->parseAnyBinaryMessage(type, payload, ws, perSocketData);
          });
    } };

  // -------------------------------
  // Register WebSocket Callbacks
  // -------------------------------

  state->app->ws<PerSocketData>(
      "/*",
      {.upgrade = onSocketUpgrade,
       .open = onSocketOpen,
       .message = onSocketMessage,
       .close = onSocketClose});

  // -----------------
  // Start Listening
  // -----------------

  state->app->listen(port, [port](auto* token) {
    if (token) {
      SPDLOG_INFO("Listening on port {}", port);
    } else {
      SPDLOG_ERROR("Failed to listen on port {}", port);
      std::exit(1);
    }
  });

  // ------------------
  // Start Game Loop
  // ------------------

  GameLoop gameLoop(state, onBroadcastMessageCallback);
  gameLoop.start();

  // ---------------------------------
  // Run WebSocket server (blocking)
  // ---------------------------------

  state->app->run();

  // ----------------------------
  // Stop Game Loop (blocking)
  // ----------------------------

  gameLoop.stop();
}
