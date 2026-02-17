#include "SerializationProtocol/Player.h"

#include "GameSharedObjects/Player.h"
#include "Player_generated.h"

std::vector<uint8_t> SerializationProtocol::serializePlayer(const Player& p) {
  flatbuffers::FlatBufferBuilder builder;
  auto name = builder.CreateString(p.name);
  auto fbPlayer = SerializationProtocolFlatbuffer::CreatePlayer(builder, name, p.messagesSent);
  builder.Finish(fbPlayer);
  return std::vector<uint8_t>(
      builder.GetBufferPointer(),
      builder.GetBufferPointer() + builder.GetSize());
}

Player SerializationProtocol::deserializePlayer(const uint8_t* data, size_t size) {
  auto fbPlayer = SerializationProtocolFlatbuffer::GetPlayer(data);
  Player p;
  p.name = fbPlayer->name()->str();
  p.messagesSent = fbPlayer->messages_sent();
  return p;
}