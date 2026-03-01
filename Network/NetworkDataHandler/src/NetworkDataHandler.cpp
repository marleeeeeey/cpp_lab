#include "NetworkDataHandler.h"

#include <spdlog/spdlog.h>

void NetworkDataHandler::registerCallbackForBinaryMessageWithType(PayloadType type, OnBinaryMessageCallback callback) {
  messageCallbacks_[type] = callback;
}

void NetworkDataHandler::registerCallbackForTextMessages(OnTextMessageCallback callback) {
  textMessageCallback_ = callback;
}

void NetworkDataHandler::notifyAboutBinaryMessage(PayloadView message) {
  parseBinaryMessage(
      message,
      [this](const PayloadType type, PayloadView payload) {
        // Search for handler
        auto it = messageCallbacks_.find(type);
        if (it != messageCallbacks_.end()) {
          it->second(type, payload);
        } else {
          SPDLOG_WARN("No handler for message type: {}", type);
        }
        SPDLOG_TRACE("Message type {} received", type);
      });
}

void NetworkDataHandler::notifyAboutTextMessage(std::string_view message) {
  if (textMessageCallback_) {
    textMessageCallback_(message);
  }
}

// Symmetric decription for INetworkDataHandler::parseBinaryMessage
std::vector<std::uint8_t> NetworkDataHandler::makeBinaryMessage(PayloadType type, PayloadView payload) const {
  if (payload.size() > 0xFFFFu) {
    SPDLOG_ERROR("Payload too large for uint16 size field");
    throw std::runtime_error("Payload too large for uint16 size field");
  }

  const std::uint16_t payloadSize = static_cast<std::uint16_t>(payload.size());

  std::vector<std::uint8_t> message;
  message.resize(sizeof(type) + sizeof(payloadSize) + payload.size());

  std::memcpy(message.data(), &type, sizeof(type));
  std::memcpy(message.data() + sizeof(type), &payloadSize, sizeof(payloadSize));
  std::memcpy(message.data() + sizeof(type) + sizeof(payloadSize), payload.data(), payload.size());

  return message;
}