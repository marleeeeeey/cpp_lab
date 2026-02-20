#pragma once

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

#include "ChatMessage_generated.h"
#include "GameSharedObjects/ChatMessage.h"
#include "GameSharedObjects/Player.h"
#include "Player_generated.h"

namespace FlatBufferWrappers {

// -------------
// glm::vec2
// -------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const glm::vec2& vec2) {
  return GameSerializationFlatbuffer::Vec2{vec2.x, vec2.y};
}

glm::vec2 deserialize(const GameSerializationFlatbuffer::Vec2* fbVec2) {
  glm::vec2 vec2 = {fbVec2->x(), fbVec2->y()};
  return vec2;
}

// -------------
// Player
// -------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const Player& player) {
  auto fbName = builder.CreateString(player.name);
  auto fbPosition = serialize(builder, player.position);
  auto fbPlayer = GameSerializationFlatbuffer::CreatePlayer(
      builder, fbName, player.messagesSent, &fbPosition);
  return fbPlayer;
}

Player deserialize(const GameSerializationFlatbuffer::Player* fbPlayer) {
  Player player;
  player.name = fbPlayer->name()->str();
  player.messagesSent = fbPlayer->messages_sent();
  player.position = deserialize(fbPlayer->position());
  return player;
}

// -------------
// Chat Message
// -------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const ChatMessage& chatMessage) {
  auto fbPlayer = serialize(builder, chatMessage.sender);
  auto message = builder.CreateString(chatMessage.message);
  auto sentTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                           chatMessage.sentTimestamp.time_since_epoch())
                           .count();
  auto receivedTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                               chatMessage.receivedTimestamp.time_since_epoch())
                               .count();

  auto fbChatMessage = GameSerializationFlatbuffer::CreateChatMessage(
      builder, fbPlayer, message, sentTimestamp, receivedTimestamp);

  return fbChatMessage;
}

ChatMessage deserialize(const GameSerializationFlatbuffer::ChatMessage* fbChatMessage) {
  ChatMessage chatMessage;
  chatMessage.sender = deserialize(fbChatMessage->sender());
  chatMessage.message = fbChatMessage->message()->str();
  chatMessage.sentTimestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(
      std::chrono::milliseconds(fbChatMessage->sent_timestamp()));
  chatMessage.receivedTimestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(
      std::chrono::milliseconds(fbChatMessage->received_timestamp()));

  return chatMessage;
}

}  // namespace FlatBufferWrappers
