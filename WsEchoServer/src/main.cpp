#include <App.h>
#include <spdlog/spdlog.h>

#include <array>
#include <magic_enum/magic_enum.hpp>
#include <memory>
#include <string>
#include <string_view>

#include "CommandLineParser.h"
#include "GameMessageTypes/GameMessageTypes.h"
#include "GameSharedObjects/ChatMessage.h"
#include "GetClientIpText.h"
#include "HumanHash.h"
#include "NetworkDataHandler/INetworkDataHandler.h"
#include "PerSocketData.h"
#include "SerializationProtocol/SerializationProtocol.h"
#include "ServerState.h"
#include "WsCloseCodeToText.h"

int main(int argc, char** argv) {
  // Example:
  // [2026-02-03 03:43:25.044] [info] WsEchoServer starting...
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

  // Uncomment the next line for Debug
  // spdlog::set_level(spdlog::level::trace);

  SPDLOG_INFO("");
  SPDLOG_INFO("---------------------------");
  SPDLOG_INFO("WsEchoServer starting...   ");
  SPDLOG_INFO("---------------------------");
  SPDLOG_INFO("");

  CommandLineParser commandLineParser(argc, argv);
  int port = commandLineParser.getPort();
  auto state = std::make_shared<ServerState>();
  auto app = state->app;

  // --------------------------
  // Set WebSockets Callbacks
  // --------------------------

  app->ws<PerSocketData>(
      "/*",
      {//
       //
       // ---------
       // Upgrade
       // ---------

       .upgrade = [](uWS::HttpResponse<false>* response, uWS::HttpRequest* request, struct us_socket_context_t* context) {

         // Set callback "onAborted"
         response->onAborted([]() { SPDLOG_WARN("upgrade aborted by client"); });

         // Get IP address hided by nginx
         auto socketIpText = response->getRemoteAddressAsText();
         const auto clientIpRaw = getClientIpText(request, socketIpText);

         // Calculate a human-readable name as hash from client addr
         auto clientAddr = convertKnownIpToShortName(clientIpRaw);
         auto humanName = HumanHash::getShortHumanName(clientIpRaw);
         SPDLOG_INFO("ws.upgrade. clientIp={}, setName={}, socketIp={}, clientIpRaw={}, ",
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
           context); },

       // ---------
       // Open
       // ---------

       .open = [state](auto* ws) {
         PerSocketData* perSocketData = (PerSocketData*)ws->getUserData();
         ws->subscribe(state->getBroadcastTopicName());
         SPDLOG_INFO("ws.open");
         state->incrementNumberOfClients(); },

       // ---------
       // Message
       // ---------

       .message = [state](auto* ws, std::string_view msg, uWS::OpCode op) {
         PerSocketData* perSocketData = (PerSocketData*)ws->getUserData();
         auto& player = perSocketData->player;

         // Update message count
         player.messagesSent++;

         if (op == uWS::OpCode::TEXT) {
           std::string personWithMessage = player.name + ": " + std::string(msg);
           state->app->publish(state->getBroadcastTopicName(), personWithMessage, op, false);
         }

         if (op == uWS::OpCode::BINARY) {
           std::vector<uint8_t> binaryMsg = std::vector<uint8_t>(msg.begin(), msg.end());  // TODO: remove unnecessary copy
           state->networkDataHandler->parseBinaryMessage(
               binaryMsg,
               [&](const INetworkDataHandler::MessageType type, const std::vector<uint8_t>& payload) {
                 // -------------------
                 // Repack payload
                 // -------------------

                 ChatMessage chatMessage = SerializationProtocol::deserializeChatMessage(payload);
                 chatMessage.sender = player;  // Update sender data. Keep timestamp data and msg the same.
                 auto replyPayload = SerializationProtocol::serializeChatMessage(chatMessage);

                 // -----------------------------
                 // Add type to reply and send
                 // -----------------------------

                 auto typedPayload = state->networkDataHandler->addTypeForBinaryMessage(GMT_ChatMessage, replyPayload);
                 std::string_view messageStringView(reinterpret_cast<const char*>(typedPayload.data()), typedPayload.size());
                 state->app->publish(state->getBroadcastTopicName(), messageStringView, op, false);  // broadcast
               });
         }

         SPDLOG_INFO("ws.message. {}: {}. op={}", perSocketData->player.name, msg, magic_enum::enum_name(op)); },

       // ---------
       // Close
       // ---------

       .close = [state](auto* ws, int code, std::string_view msg) {
         PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();
         msg = msg.empty() ? wsCloseCodeToText(code) : msg;
         SPDLOG_INFO("ws.close. ip={}, name={}, code={}, msg={}",
                     perSocketData->clientIp, perSocketData->player.name, code, msg);
         state->decrementNumberOfClients(); }});

  // -----------------
  // Start Listening
  // -----------------

  app->listen(port, [port](auto* token) {
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

  app->run();
}
