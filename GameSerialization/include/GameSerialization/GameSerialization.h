#pragma once

#include <cstdint>
#include <vector>

#include "GameShared/ChatMessage.h"
#include "GameShared/InputPacket.h"
#include "GameShared/Player.h"
#include "GameShared/TimeStamp.h"
#include "GameShared/WorldSnapshot.h"
#include "MemcpySerialization.h"

namespace GameSerialization {

// ----------------
// InputPacket
// ----------------

std::vector<uint8_t> serializeInputPacket(const InputPacket& item);
InputPacket deserializeInputPacket(const std::vector<uint8_t>& payload);

// ----------------
// TimeStamp
// ----------------

std::vector<uint8_t> serializeTimeStamp(const TimeStamp& p);
TimeStamp deserializeTimeStamp(const std::vector<uint8_t>& payload);

// ----------------
// Player
// ----------------

std::vector<uint8_t> serializePlayer(const Player& p);
Player deserializePlayer(const std::vector<uint8_t>& payload);

// ----------------
// ChatMessage
// ----------------

std::vector<uint8_t> serializeChatMessage(const ChatMessage& chatMessage);
ChatMessage deserializeChatMessage(const std::vector<uint8_t>& payload);

// ----------------
// WorldSnapshot
// ----------------

std::vector<uint8_t> serializeWorldSnapshot(const WorldSnapshot& worldSnapshot);
WorldSnapshot deserializeWorldSnapshot(const std::vector<uint8_t>& payload);

};  // namespace GameSerialization
