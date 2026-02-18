#include "SerializationProtocol/SerializationProtocol.h"

#include "FlatBufferTemplates.h"

// ----------------
// Player
// ----------------

#include "GameSharedObjects/Player.h"
#include "Player_generated.h"

std::vector<uint8_t> SerializationProtocol::serializePlayer(const Player& player) {
  return FlatBufferTemplates::serialize(player);
}

Player SerializationProtocol::deserializePlayer(const std::vector<uint8_t>& payload) {
  auto fbPlayer = SerializationProtocolFlatbuffer::GetPlayer(payload.data());
  return FlatBufferWrappers::deserialize(fbPlayer);
}

// -------------------
// Chat Message
// -------------------

#include "ChatMessage_generated.h"
#include "GameSharedObjects/ChatMessage.h"

std::vector<uint8_t> SerializationProtocol::serializeChatMessage(const ChatMessage& chatMessage) {
  return FlatBufferTemplates::serialize(chatMessage);
}

ChatMessage SerializationProtocol::deserializeChatMessage(const std::vector<uint8_t>& payload) {
  auto fbChatMessage = SerializationProtocolFlatbuffer::GetChatMessage(payload.data());
  return FlatBufferWrappers::deserialize(fbChatMessage);
}