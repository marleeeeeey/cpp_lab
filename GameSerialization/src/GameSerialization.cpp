#include "GameSerialization/GameSerialization.h"

#include "FlatBufferTemplates.h"

// ----------------
// Player
// ----------------

#include "GameSharedObjects/Player.h"
#include "Player_generated.h"

std::vector<uint8_t> GameSerialization::serializePlayer(const Player& player) {
  return FlatBufferTemplates::serialize(player);
}

Player GameSerialization::deserializePlayer(const std::vector<uint8_t>& payload) {
  auto fbPlayer = GameSerializationFlatbuffer::GetPlayer(payload.data());
  return FlatBufferWrappers::deserialize(fbPlayer);
}

// -------------------
// Chat Message
// -------------------

#include "ChatMessage_generated.h"
#include "GameSharedObjects/ChatMessage.h"

std::vector<uint8_t> GameSerialization::serializeChatMessage(const ChatMessage& chatMessage) {
  return FlatBufferTemplates::serialize(chatMessage);
}

ChatMessage GameSerialization::deserializeChatMessage(const std::vector<uint8_t>& payload) {
  auto fbChatMessage = GameSerializationFlatbuffer::GetChatMessage(payload.data());
  return FlatBufferWrappers::deserialize(fbChatMessage);
}