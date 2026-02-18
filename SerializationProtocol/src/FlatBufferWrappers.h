#pragma once

#include <spdlog/spdlog.h>

#include "ChatMessage_generated.h"
#include "GameSharedObjects/ChatMessage.h"
#include "GameSharedObjects/Player.h"
#include "Player_generated.h"

namespace FlatBufferWrappers {

// -------------
// Player
// -------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const Player& player) {
  auto name = builder.CreateString(player.name);
  auto fbPlayer = SerializationProtocolFlatbuffer::CreatePlayer(builder, name, player.messagesSent);
  return fbPlayer;
}

Player deserialize(const SerializationProtocolFlatbuffer::Player* fbPlayer) {
  Player player;
  player.name = fbPlayer->name()->str();
  player.messagesSent = fbPlayer->messages_sent();
  return player;
}

// -------------
// Chat Message
// -------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const ChatMessage& chatMessage) {
  auto fbPlayer = serialize(builder, chatMessage.sender);
  auto message = builder.CreateString(chatMessage.message);
  auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                       chatMessage.timestamp.time_since_epoch())
                       .count();

  auto fbChatMessage = SerializationProtocolFlatbuffer::CreateChatMessage(
      builder, fbPlayer, message, timestamp);

  return fbChatMessage;
}

ChatMessage deserialize(const SerializationProtocolFlatbuffer::ChatMessage* fbChatMessage) {
  ChatMessage chatMessage;
  chatMessage.sender = deserialize(fbChatMessage->sender());
  chatMessage.message = fbChatMessage->message()->str();
  chatMessage.timestamp = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(
      std::chrono::milliseconds(fbChatMessage->timestamp()));

  return chatMessage;
}

}  // namespace FlatBufferWrappers
