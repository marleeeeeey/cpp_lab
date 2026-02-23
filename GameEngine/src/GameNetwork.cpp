#include "GameNetwork.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

#include "GameMessageTypes/GameMessageTypes.h"
#include "GameSerialization/GameSerialization.h"

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

void GameNetwork::sendPlayer(const Player& player) {
  // IMPROVE: do both operations in one call and one memory allocation. Here and other similar places,
  auto payload = GameSerialization::serializePlayer(player);
  payload = networkDataHandler_->addTypeForBinaryMessage(GMT_AnyPlayerDataUpdated, payload);
  autoReconnectionNetwork_->sendBinary(payload);
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

void GameNetwork::initNetworkDataHandlers_() {
  networkDataHandler_ = INetworkDataHandler::create();

  networkDataHandler_->registerCallbackForTextMessages(
      [this](std::string_view textMessage) {
        ChatMessage chatMessage{
            .sender = Player{
                .name = "Anonymous",
                .messagesSent = 0,
            },
            .message = std::string(textMessage),
        };
        if (auto render = chatRenderer_.lock()) {
          render->addMessage(chatMessage);
        }
        SPDLOG_INFO("Text Message received: {}", chatMessage.message);
      });

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
      GMT_AnyPlayerDataUpdated,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        auto player = GameSerialization::deserializePlayer(payload);
        if (auto renderer = gameWorldRenderer_.lock()) {
          if (player.name == renderer->myPlayer.name) {
            return;  // don't update my own data
          }
          renderer->otherPlayers[player.name] = player;
        }
        SPDLOG_DEBUG("Message type {} received", type);
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_AssignNameToThePlayer,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        auto player = GameSerialization::deserializePlayer(payload);
        if (auto renderer = gameWorldRenderer_.lock()) {
          renderer->myPlayer = player;
        }
        if (auto gameWorld = gameWorld_.lock()) {
          gameWorld->setPlayerRandomPosition();
        }

        SPDLOG_INFO("Your name is {}. Welcome!", player.name);

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