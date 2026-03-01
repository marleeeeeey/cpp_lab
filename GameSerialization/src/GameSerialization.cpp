#include "GameSerialization/GameSerialization.h"

#include "ChatMessage_generated.h"
#include "FlatBufferTemplates.h"
#include "Player_generated.h"

// ----------------
// InputPacket
// ----------------

std::vector<uint8_t> GameSerialization::serializeInputPacket(const InputPacket& item) {
  return GameSerialization::serializeMemcpy(item);
}

InputPacket GameSerialization::deserializeInputPacket(PayloadView payload) {
  return GameSerialization::deserializeMemcpy<InputPacket>(payload);
}

// ----------------
// Player
// ----------------

std::vector<uint8_t> GameSerialization::serializePlayer(const Player& player) {
  return FlatBufferTemplates::serialize(player);
}

Player GameSerialization::deserializePlayer(PayloadView payload) {
  auto fbPlayer = GameSerializationFlatbuffer::GetPlayer(payload.data());
  return FlatBufferWrappers::deserialize(fbPlayer);
}

// -------------------
// Chat Message
// -------------------

std::vector<uint8_t> GameSerialization::serializeChatMessage(const ChatMessage& chatMessage) {
  return FlatBufferTemplates::serialize(chatMessage);
}

ChatMessage GameSerialization::deserializeChatMessage(PayloadView payload) {
  auto fbChatMessage = GameSerializationFlatbuffer::GetChatMessage(payload.data());
  return FlatBufferWrappers::deserialize(fbChatMessage);
}

// ----------------
// WorldSnapshot
// ----------------

std::vector<uint8_t> GameSerialization::serializeWorldSnapshot(const WorldSnapshot& worldSnapshot) {
  return FlatBufferTemplates::serialize(worldSnapshot);
}

WorldSnapshot GameSerialization::deserializeWorldSnapshot(PayloadView payload) {
  auto fbWorldSnapshot = GameSerializationFlatbuffer::GetWorldSnapshot(payload.data());
  return FlatBufferWrappers::deserialize(fbWorldSnapshot);
}
