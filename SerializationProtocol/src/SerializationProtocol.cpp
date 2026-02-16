#include "Player_generated.h"

void test() {
  flatbuffers::FlatBufferBuilder builder;
  using namespace SerializationProtocol;

  // -------------
  // Serialize
  // -------------

  auto playerName = builder.CreateString("John");
  auto player = CreatePlayer(builder, playerName);
  builder.Finish(player);

  // -------------
  // Deserialize
  // -------------

  auto player2 = GetPlayer(builder.GetBufferPointer());
  auto playerName2 = player2->name()->str();
}