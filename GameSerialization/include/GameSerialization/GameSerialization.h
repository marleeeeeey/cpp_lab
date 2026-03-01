#pragma once

#include <cstdint>
#include <vector>

#include "GameShared/ChatMessage.h"
#include "GameShared/InputPacket.h"
#include "GameShared/Player.h"
#include "GameShared/WorldSnapshot.h"
#include "GlobalTypes/GlobalTypes.h"
#include "MemcpySerialization.h"

namespace GameSerialization {

// ----------------
// InputPacket
// ----------------

std::vector<uint8_t> serializeInputPacket(const InputPacket& item);
InputPacket deserializeInputPacket(PayloadView payload);

// ----------------
// Player
// ----------------

std::vector<uint8_t> serializePlayer(const Player& p);
Player deserializePlayer(PayloadView payload);

// ----------------
// ChatMessage
// ----------------

std::vector<uint8_t> serializeChatMessage(const ChatMessage& chatMessage);
ChatMessage deserializeChatMessage(PayloadView payload);

// ----------------
// WorldSnapshot
// ----------------

std::vector<uint8_t> serializeWorldSnapshot(const WorldSnapshot& worldSnapshot);
WorldSnapshot deserializeWorldSnapshot(PayloadView payload);

};  // namespace GameSerialization
