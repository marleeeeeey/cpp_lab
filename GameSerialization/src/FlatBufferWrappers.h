#pragma once

#include <spdlog/spdlog.h>

#include <cstdint>
#include <glm/glm.hpp>

#include "ChatMessage_generated.h"
#include "GameShared/ChatMessage.h"
#include "GameShared/Player.h"
#include "GameShared/TimeStamp.h"
#include "GameShared/WorldSnapshot.h"
#include "Player_generated.h"
#include "WorldSnapshot_generated.h"

namespace FlatBufferWrappers {

// -------------
// TimeStamp
// -------------

std::uint64_t serialize(const TimeStamp& timeStamp) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(timeStamp.time_since_epoch()).count();
}

TimeStamp deserialize(const std::uint64_t fbTimeStamp) {
  return std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>(
      std::chrono::milliseconds(fbTimeStamp));
}

// -------------
// glm::vec2
// -------------

auto serialize(const glm::vec2& vec2) {
  return GameSerializationFlatbuffer::Vec2{vec2.x, vec2.y};
}

glm::vec2 deserialize(const GameSerializationFlatbuffer::Vec2* fbVec2) {
  glm::vec2 vec2 = {fbVec2->x(), fbVec2->y()};
  return vec2;
}

// -------------
// Player
// -------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const Player& player) {
  auto fbName = builder.CreateString(player.name);
  auto fbPosition = serialize(player.state.position);
  auto fbPlayer = GameSerializationFlatbuffer::CreatePlayer(
      builder, fbName, player.messagesSent, &fbPosition);
  return fbPlayer;
}

Player deserialize(const GameSerializationFlatbuffer::Player* fbPlayer) {
  Player player;
  player.name = fbPlayer->name()->str();
  player.messagesSent = fbPlayer->messages_sent();
  player.state.position = deserialize(fbPlayer->position());
  return player;
}

// -------------
// Chat Message
// -------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const ChatMessage& chatMessage) {
  auto fbPlayer = serialize(builder, chatMessage.sender);
  auto message = builder.CreateString(chatMessage.message);
  auto sentTimestamp = serialize(chatMessage.sentTimestamp);
  auto receivedTimestamp = serialize(chatMessage.receivedTimestamp);

  auto fbChatMessage = GameSerializationFlatbuffer::CreateChatMessage(
      builder, fbPlayer, message, sentTimestamp, receivedTimestamp);

  return fbChatMessage;
}

ChatMessage deserialize(const GameSerializationFlatbuffer::ChatMessage* fbChatMessage) {
  ChatMessage chatMessage;
  chatMessage.sender = deserialize(fbChatMessage->sender());
  chatMessage.message = fbChatMessage->message()->str();
  chatMessage.sentTimestamp = deserialize(fbChatMessage->sent_timestamp());
  chatMessage.receivedTimestamp = deserialize(fbChatMessage->received_timestamp());
  return chatMessage;
}

// ------------------
// PlayerSnapshot
// ------------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const PlayerSnapshot& snapshot) {
  auto fbPosition = serialize(snapshot.position);

  return GameSerializationFlatbuffer::CreatePlayerSnapshot(builder, snapshot.id, &fbPosition);
}

PlayerSnapshot deserialize(const GameSerializationFlatbuffer::PlayerSnapshot* fbSnapshot) {
  PlayerSnapshot snapshot;

  snapshot.id = fbSnapshot->id();
  snapshot.position = deserialize(fbSnapshot->position());

  return snapshot;
}

// -----------------
// WorldSnapshot
// -----------------

auto serialize(flatbuffers::FlatBufferBuilder& builder, const WorldSnapshot& worldSnapshot) {
  std::vector<flatbuffers::Offset<GameSerializationFlatbuffer::PlayerSnapshot>> playerOffsets;

  playerOffsets.reserve(worldSnapshot.players.size());

  for (const auto& player : worldSnapshot.players) {
    playerOffsets.push_back(serialize(builder, player));
  }

  auto fbPlayers = builder.CreateVector(playerOffsets);

  return GameSerializationFlatbuffer::CreateWorldSnapshot(builder, worldSnapshot.serverTick, fbPlayers);
}

WorldSnapshot deserialize(const GameSerializationFlatbuffer::WorldSnapshot* fbWorld) {
  WorldSnapshot worldSnapshot;

  worldSnapshot.serverTick = fbWorld->server_tick();

  auto fbPlayers = fbWorld->players();
  if (fbPlayers) {
    worldSnapshot.players.reserve(fbPlayers->size());

    for (const auto* fbPlayer : *fbPlayers) {
      worldSnapshot.players.push_back(deserialize(fbPlayer));
    }
  }

  return worldSnapshot;
}

}  // namespace FlatBufferWrappers
