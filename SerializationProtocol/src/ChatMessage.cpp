#include "SerializationProtocol/ChatMessage.h"

#include "ChatMessage_generated.h"
#include "GameSharedObjects/ChatMessage.h"
#include "Player_generated.h"

std::vector<uint8_t> SerializationProtocol::serializeChatMessage(const ChatMessage& d) {
  flatbuffers::FlatBufferBuilder builder;
  auto name = builder.CreateString(d.sender.name);
  // TODO: Think how to increase code reusing.
  // Because code for player creation already exists in "serializePlayer" method.
  auto fbPlayer = SerializationProtocolFlatbuffer::CreatePlayer(builder, name, d.sender.messagesSent);
  auto message = builder.CreateString(d.message);
  auto fbChatMessage = SerializationProtocolFlatbuffer::CreateChatMessage(builder, fbPlayer, message);
  builder.Finish(fbChatMessage);
  return std::vector<uint8_t>(
      builder.GetBufferPointer(),
      builder.GetBufferPointer() + builder.GetSize());
}

ChatMessage SerializationProtocol::deserializeChatMessage(const std::vector<uint8_t>& payload) {
  auto data = payload.data();
  auto fbChatMessage = SerializationProtocolFlatbuffer::GetChatMessage(data);
  ChatMessage chatMessage;
  chatMessage.sender.name = fbChatMessage->sender()->name()->str();
  chatMessage.sender.messagesSent = fbChatMessage->sender()->messages_sent();
  chatMessage.message = fbChatMessage->message()->str();
  return chatMessage;
}