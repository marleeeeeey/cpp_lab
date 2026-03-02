#include "ClientNetwork.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

#include "GameSerialization/GameSerialization.h"
#include "GameSerialization/MemcpySerialization.h"
#include "GameShared/GameMessageTypes.h"
#include "GlobalTypes/GlobalTypes.h"

ClientNetwork::ClientNetwork(
    const std::string& url,
    std::weak_ptr<IDebugRender> debugRender,
    std::weak_ptr<IChatRenderer> chatRenderer,
    std::weak_ptr<IGameWorldRenderer> gameWorldRenderer,
    std::weak_ptr<SnowflakesSimulation> snowflakesSimulation,
    std::weak_ptr<GameTimer> gameTimer) {
  url_ = url;
  debugRender_ = debugRender;
  chatRenderer_ = chatRenderer;
  gameWorldRenderer_ = gameWorldRenderer;
  snowflakesSimulation_ = snowflakesSimulation;
  gameTimer_ = gameTimer;

  initNetworkDataHandlers_();
  initAutoReconnectionNetwork_();
}

void ClientNetwork::start() {
  autoReconnectionNetwork_->start();
}

void ClientNetwork::iterate() {
  autoReconnectionNetwork_->iterate();
}

void ClientNetwork::stop() {
  autoReconnectionNetwork_->stop();
}

void ClientNetwork::sendChatMessage(const ChatMessage& message) {
  auto payload = GameSerialization::serializeChatMessage(message);
  payload = networkDataHandler_->makeBinaryMessage(GMT_ChatMessage, payload);
  autoReconnectionNetwork_->sendBinary(payload);
}

void ClientNetwork::sendPingFromClient() {
  auto now = std::chrono::system_clock::now();
  auto message = networkDataHandler_->makeBinaryMessageMemcpy(GMT_PingFromClient, now);
  autoReconnectionNetwork_->sendBinary(message);
}

void ClientNetwork::sendInputPacketFromClient(const InputPacket& inputPacket) {
  auto message = networkDataHandler_->makeBinaryMessageMemcpy(GMT_InputDataFromClient, inputPacket);
  autoReconnectionNetwork_->sendBinary(message);
}

void ClientNetwork::initNetworkDataHandlers_() {
  networkDataHandler_ = INetworkDataHandler::create();

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_ChatMessage,
      [this](PayloadType type, PayloadView payload) {
        auto newChatMessage = GameSerialization::deserializeChatMessage(payload);
        newChatMessage.receivedTimestamp = std::chrono::system_clock::now();
        if (auto renderer = chatRenderer_.lock()) {
          renderer->addMessage(newChatMessage);
        }
        SPDLOG_TRACE("Message type {} received: {}", type, newChatMessage.message);
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_NumberOfClients,
      [this](PayloadType type, PayloadView payload) {
        int userNumber = GameSerialization::deserializeMemcpy<int>(payload);
        // set and log
        if (auto renderer = debugRender_.lock()) {
          renderer->addStaticLine("user count", std::format("Users: {}", userNumber));
        }
        SPDLOG_TRACE("Message type {} received: {}", type, userNumber);
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_PlayerIdFromServer,
      [this](PayloadType type, PayloadView payload) {
        auto playerId = GameSerialization::deserializeMemcpy<PlayerId>(payload);

        if (auto renderer = gameWorldRenderer_.lock()) {
          renderer->myPlayerId = playerId;
        }

        if (auto debugRender = debugRender_.lock()) {
          debugRender->addStaticLine("playerId", std::format("Player Id: {}", playerId));
        }

        sendPingFromClient();
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_PingFromClient,
      [this](PayloadType type, PayloadView payload) {
        auto creationTime = GameSerialization::deserializeMemcpy<TimeStamp>(payload);
        auto now = std::chrono::system_clock::now();
        if (auto renderer = debugRender_.lock()) {
          auto pingMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - creationTime).count();
          renderer->addStaticLine("ping", std::format("Ping: {} ms", pingMs));
        }
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_WorldSnapshot,
      [this](PayloadType type, PayloadView payload) {
        auto worldSnapshot = GameSerialization::deserializeWorldSnapshot(payload);

        if (auto gameRenderer = gameWorldRenderer_.lock()) {
          gameRenderer->worldSnapshot = worldSnapshot;
        }

        if (auto renderer = debugRender_.lock()) {
          renderer->addStaticLine("serverTick", std::format("Tick: {}", worldSnapshot.serverTick));
        }
      });
}

void ClientNetwork::initAutoReconnectionNetwork_() {
  autoReconnectionNetwork_ = IAutoReconnectionNetwork::create();
  autoReconnectionNetwork_->init(
      url_,
      [this](std::string_view textMessage) {
        SPDLOG_TRACE("Received text message: {}", textMessage);
        networkDataHandler_->notifyAboutTextMessage(textMessage);
      },
      [this](std::vector<uint8_t> binaryMessage) {
        SPDLOG_TRACE("Received binary message");
        networkDataHandler_->notifyAboutBinaryMessage(binaryMessage);
      },
      [this](IAutoReconnectionNetwork::State newState) {
        if (auto render = debugRender_.lock()) {
          render->addStaticLine("connection state", std::format("State: {}", magic_enum::enum_name(newState)));
        }
      });
}