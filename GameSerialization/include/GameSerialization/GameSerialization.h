#pragma once

#include <cstdint>
#include <vector>

#include "GameSharedObjects/TimeStamp.h"

// -------------------------
// Forward Declarations
// -------------------------

struct Player;
struct ChatMessage;

namespace GameSerialization {

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

};  // namespace GameSerialization
