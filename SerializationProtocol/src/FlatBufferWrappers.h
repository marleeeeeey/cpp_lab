#pragma once

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
  auto fbChatMessage = SerializationProtocolFlatbuffer::CreateChatMessage(builder, fbPlayer, message);
  return fbChatMessage;
}

ChatMessage deserialize(const SerializationProtocolFlatbuffer::ChatMessage* fbChatMessage) {
  ChatMessage chatMessage;
  chatMessage.sender = deserialize(fbChatMessage->sender());
  chatMessage.message = fbChatMessage->message()->str();
  return chatMessage;
}

}  // namespace FlatBufferWrappers
