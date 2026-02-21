#include "GameSerialization/GameSerialization.h"

#include "FlatBufferTemplates.h"

// -------------------
// TimeStamp
// -------------------

#include "GameSharedObjects/TimeStamp.h"

std::vector<uint8_t> GameSerialization::serializeTimeStamp(const TimeStamp& p) {
  const std::uint64_t ms = FlatBufferWrappers::serialize(p);
  std::vector<std::uint8_t> payload(sizeof(ms));
  std::memcpy(payload.data(), &ms, sizeof(ms));
  return payload;
}

TimeStamp GameSerialization::deserializeTimeStamp(const std::vector<uint8_t>& payload) {
  std::uint64_t ms = 0;
  std::memcpy(&ms, payload.data(), sizeof(ms));
  return FlatBufferWrappers::deserialize(ms);
}

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