#pragma once

#include <cstdint>
#include <vector>

struct Player;

namespace SerializationProtocol {

std::vector<uint8_t> serializePlayer(const Player& p);
Player deserializePlayer(const uint8_t* data, size_t size);

}  // namespace SerializationProtocol