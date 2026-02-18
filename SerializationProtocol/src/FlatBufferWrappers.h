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
  auto sentTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                           chatMessage.sentTimestamp.time_since_epoch())
                           .count();
  auto receivedTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                               chatMessage.receivedTimestamp.time_since_epoch())
                               .count();

  auto fbChatMessage = SerializationProtocolFlatbuffer::CreateChatMessage(
      builder, fbPlayer, message, sentTimestamp, receivedTimestamp);

  return fbChatMessage;
}

ChatMessage deserialize(const SerializationProtocolFlatbuffer::ChatMessage* fbChatMessage) {
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
