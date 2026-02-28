#include "GameNetwork.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

#include "GameSerialization/GameSerialization.h"
#include "GameShared/GameMessageTypes.h"

GameNetwork::GameNetwork(
    const std::string& url,
    std::weak_ptr<IDebugRender> debugRender,
    std::weak_ptr<IChatRenderer> chatRenderer,
    std::weak_ptr<IGameWorldRenderer> gameWorldRenderer,
    std::weak_ptr<GameWorld> gameWorld,
    std::weak_ptr<GameTimer> gameTimer) {
  url_ = url;
  debugRender_ = debugRender;
  chatRenderer_ = chatRenderer;
  gameWorldRenderer_ = gameWorldRenderer;
  gameWorld_ = gameWorld;
  gameTimer_ = gameTimer;

  initNetworkDataHandlers_();
  initAutoReconnectionNetwork_();
}

void GameNetwork::start() {
  autoReconnectionNetwork_->start();
}

void GameNetwork::iterate() {
  autoReconnectionNetwork_->iterate();
}

void GameNetwork::stop() {
  autoReconnectionNetwork_->stop();
}

void GameNetwork::sendChatMessage(const ChatMessage& message) {
  auto payload = GameSerialization::serializeChatMessage(message);
  payload = networkDataHandler_->addTypeForBinaryMessage(GMT_ChatMessage, payload);
  autoReconnectionNetwork_->sendBinary(payload);
}

void GameNetwork::sendPingFromClient() {
  auto now = std::chrono::system_clock::now();
  auto payload = GameSerialization::serializeTimeStamp(now);
  payload = networkDataHandler_->addTypeForBinaryMessage(GMT_PingFromClient, payload);
  autoReconnectionNetwork_->sendBinary(payload);
}

void GameNetwork::sendInputPacketFromClient(const InputPacket& inputPacket) {
  auto payload = GameSerialization::serializeInputPacket(inputPacket);
  payload = networkDataHandler_->addTypeForBinaryMessage(GMT_InputDataFromClient, payload);
  autoReconnectionNetwork_->sendBinary(payload);
}

void GameNetwork::initNetworkDataHandlers_() {
  networkDataHandler_ = INetworkDataHandler::create();

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_ChatMessage,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        auto newChatMessage = GameSerialization::deserializeChatMessage(payload);
        newChatMessage.receivedTimestamp = std::chrono::system_clock::now();
        if (auto renderer = chatRenderer_.lock()) {
          renderer->addMessage(newChatMessage);
        }
        SPDLOG_TRACE("Message type {} received: {}", type, newChatMessage.message);
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_NumberOfClients,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        // unpack number of users
        int receivedNumber = 0;
        std::memcpy(&receivedNumber, payload.data(), sizeof(receivedNumber));

        // set and log
        if (auto renderer = debugRender_.lock()) {
          renderer->addStaticLine("user count", std::format("Users: {}", receivedNumber));
        }
        SPDLOG_TRACE("Message type {} received: {}", type, receivedNumber);
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_PlayerIdFromServer,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        auto player = GameSerialization::deserializePlayer(payload);
        if (auto renderer = gameWorldRenderer_.lock()) {
          renderer->myPlayerId = player.id;
        }
        if (auto debugRender = debugRender_.lock()) {
          debugRender->addStaticLine("name", std::format("Name: {}", player.name));
        }

        sendPingFromClient();
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_PingFromClient,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        auto creationTime = GameSerialization::deserializeTimeStamp(payload);
        auto now = std::chrono::system_clock::now();
        if (auto renderer = debugRender_.lock()) {
          auto pingMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - creationTime).count();
          renderer->addStaticLine("ping", std::format("Ping: {} ms", pingMs));
        }
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_WorldSnapshot,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        auto worldSnapshot = GameSerialization::deserializeWorldSnapshot(payload);
        SPDLOG_TRACE("Message type GMT_WorldSnapshot received", type);

        if (!worldSnapshot.players.empty()) {
          auto& p1 = worldSnapshot.players[0];
          SPDLOG_TRACE("Received WorldSnapshot: p1 pos=({},{})", p1.position.x, p1.position.y);
        }

        if (auto gameWorld = gameWorld_.lock()) {
          gameWorld->setWorldSnapshot(worldSnapshot);
        }
        if (auto gameRenderer = gameWorldRenderer_.lock()) {
          gameRenderer->worldSnapshot = worldSnapshot;
        }
      });
}

void GameNetwork::initAutoReconnectionNetwork_() {
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