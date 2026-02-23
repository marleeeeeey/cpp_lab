#include <App.h>
#include <GameMessageTypes/GameMessageTypes.h>
#include <GameSerialization/GameSerialization.h>
#include <spdlog/spdlog.h>

#include <array>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <random>
#include <string>
#include <string_view>

#include "CommandLineParser.h"
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

  // TODO create wrapper around uWS::App and make this function as interface of this wrapper
  auto sendTypedBinaryToWebSocket = [&state](GameMessageType type, auto* ws, const std::vector<uint8_t>& payload) {
    auto typedPayload = state->networkDataHandler->addTypeForBinaryMessage(type, payload);
    std::string_view stringView(reinterpret_cast<const char*>(typedPayload.data()), typedPayload.size());
    ws->send(stringView, uWS::OpCode::BINARY);
  };

  // --------------------------------------
  // Create BinaryMessageParser instance
  // --------------------------------------

  BinaryMessageParser::OnBroadcastMessageCallback onBroadcastMessageCallback =
      [&state](const INetworkDataHandler::MessageType replyType, const std::vector<uint8_t>& replyPayload) {
        auto typedPayload = state->networkDataHandler->addTypeForBinaryMessage(replyType, replyPayload);
        std::string_view messageStringView(reinterpret_cast<const char*>(typedPayload.data()), typedPayload.size());
        state->app->publish(state->getBroadcastTopicName(), messageStringView, uWS::OpCode::BINARY);  // broadcast
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
    auto ip = perSocketData->clientIp;
    auto name = player.name;
    while (state->connectedClientNames.contains(name)) {
      name = HumanHash::getShortHumanName(ip + name);
    }
    player.name = name;
    state->connectedClientNames.insert(name);
    SPDLOG_DEBUG("ws.open");
    SPDLOG_INFO("{} connected from ip {}", player.name, ip);

    auto payload = GameSerialization::serializePlayer(player);
    sendTypedBinaryToWebSocket(GMT_AssignNameToThePlayer, ws, payload);

    // TODO world state should be sent to this client. Need to save player positions on server
  };

  // ---------------------------
  // Message (incoming data)
  // ---------------------------

  auto onSocketMessage = [state](auto* ws, std::string_view msg, uWS::OpCode op) {
    PerSocketData* perSocketData = (PerSocketData*)ws->getUserData();
    auto& player = perSocketData->player;
    player.messagesSent++; // update message count; just for statistics

    // Echo any text. This is obsolete.
    if (op == uWS::OpCode::TEXT) {
      std::string personWithMessage = player.name + ": " + std::string(msg);
      state->app->publish(state->getBroadcastTopicName(), personWithMessage, op, false);
      SPDLOG_DEBUG("ws.message. {}: {}. op={}", perSocketData->player.name, msg, magic_enum::enum_name(op));
    }

    // Detect a binary message type and delegate parsing to BinaryMessageParser
    if (op == uWS::OpCode::BINARY) {
      std::vector<uint8_t> binaryMsg = std::vector<uint8_t>(msg.begin(), msg.end());  // TODO: remove unnecessary copy
      SPDLOG_DEBUG("ws.message. From {}; op={}", perSocketData->player.name, magic_enum::enum_name(op));
      state->networkDataHandler->parseBinaryMessage(
        binaryMsg,
        [&](const INetworkDataHandler::MessageType type, const std::vector<uint8_t>& payload) {
            state->binaryMessageParser->parseAnyBinaryMessage(type, payload, ws, perSocketData);
          });
    } };

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

    state->decrementNumberOfClients(); };

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

  // ---------------------------------
  // Run WebSocket server (blocking)
  // ---------------------------------

  state->app->run();
}
