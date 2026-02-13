#include "NetworkDataHandler.h"

#include <spdlog/spdlog.h>

void NetworkDataHandler::registerMessageType(MessageType type, OnMessageCallback callback) {
  messageCallbacks_[type] = callback;
}

std::vector<uint8_t> NetworkDataHandler::prepareMessage(MessageType type, const std::vector<uint8_t>& payload) {
  std::vector<uint8_t> message;
  uint16_t payloadSize = static_cast<uint16_t>(payload.size());

  // Reserve memory
  message.resize(sizeof(type) + sizeof(payloadSize) + payload.size());

  // Write type, payload size and payload
  std::memcpy(message.data(), &type, sizeof(type));
  std::memcpy(message.data() + sizeof(type), &payloadSize, sizeof(payloadSize));
  std::memcpy(message.data() + sizeof(type) + sizeof(payloadSize), payload.data(), payload.size());

  return message;
}

void NetworkDataHandler::parseMessage(const std::vector<uint8_t>& message) {
  // Read type
  MessageType type;
  std::memcpy(&type, message.data(), sizeof(type));

  // Read payload size
  uint16_t payloadSize;
  std::memcpy(&payloadSize, message.data() + sizeof(type), sizeof(payloadSize));

  // Read payload
  std::vector<uint8_t> payload(payloadSize);
  std::memcpy(payload.data(), message.data() + sizeof(type) + sizeof(payloadSize), payloadSize);

  // Search for handler
  auto it = messageCallbacks_.find(type);
  if (it != messageCallbacks_.end()) {
    it->second(type, payload);
  } else {
    SPDLOG_WARN("No handler for message type: {}", type);
  }
}