#pragma once

#include <cstdint>
#include <vector>

struct ChatMessage;

namespace SerializationProtocol {

std::vector<uint8_t> serializeChatMessage(const ChatMessage& d);
ChatMessage deserializeChatMessage(const std::vector<uint8_t>& payload);

}  // namespace SerializationProtocol