#include "GameSerialization/GameSerialization.h"

#include "ChatMessage_generated.h"
#include "FlatBufferTemplates.h"
#include "Player_generated.h"

// ----------------
// InputPacket
// ----------------

std::vector<uint8_t> GameSerialization::serializeInputPacket(const InputPacket& item) {
  std::vector<uint8_t> buffer(sizeof(InputPacket));
  std::memcpy(buffer.data(), &item, sizeof(InputPacket));
  return buffer;
}

InputPacket GameSerialization::deserializeInputPacket(const std::vector<uint8_t>& payload) {
  InputPacket item;
  std::memcpy(&item, payload.data(), sizeof(InputPacket));
  return item;
}

// -------------------
// TimeStamp
// -------------------

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

std::vector<uint8_t> GameSerialization::serializeChatMessage(const ChatMessage& chatMessage) {
  return FlatBufferTemplates::serialize(chatMessage);
}

ChatMessage GameSerialization::deserializeChatMessage(const std::vector<uint8_t>& payload) {
  auto fbChatMessage = GameSerializationFlatbuffer::GetChatMessage(payload.data());
  return FlatBufferWrappers::deserialize(fbChatMessage);
}

// ----------------
// WorldSnapshot
// ----------------

std::vector<uint8_t> GameSerialization::serializeWorldSnapshot(const WorldSnapshot& worldSnapshot) {
  return FlatBufferTemplates::serialize(worldSnapshot);
}

WorldSnapshot GameSerialization::deserializeWorldSnapshot(const std::vector<uint8_t>& payload) {
  auto fbWorldSnapshot = GameSerializationFlatbuffer::GetWorldSnapshot(payload.data());
  return FlatBufferWrappers::deserialize(fbWorldSnapshot);
}
