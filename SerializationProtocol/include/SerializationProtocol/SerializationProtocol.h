#pragma once

#include <cstdint>
#include <vector>

// -------------------------
// Forward Declarations
// -------------------------

struct Player;
struct ChatMessage;

namespace SerializationProtocol {

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

};  // namespace SerializationProtocol
