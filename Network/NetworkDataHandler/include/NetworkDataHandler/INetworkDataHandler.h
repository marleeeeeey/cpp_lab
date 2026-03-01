#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "GlobalTypes/GlobalTypes.h"

// Interface for
// 1. Adding MessageType to every binary message.
// 2. Registering callbacks for specific message types.
// Also support similar functionality for text messages but without Message Types.
// For text messages only one callback can be registered.
class INetworkDataHandler {
 public:
  // ------------
  // Factory
  // ------------

  static std::unique_ptr<INetworkDataHandler> create();
  virtual ~INetworkDataHandler() = default;

  // -----------------------
  // Signatures and Types
  // -----------------------

  // Callback signature
  using OnBinaryMessageCallback = std::function<void(PayloadType type, PayloadView payload)>;
  using OnTextMessageCallback = std::function<void(std::string_view message)>;

  // ------------
  // Interface
  // ------------

  // Register a callback for a binary with specific message type
  virtual void registerCallbackForBinaryMessageWithType(PayloadType type, OnBinaryMessageCallback callback) = 0;

  // Register union callback for all text messages (obsolete method)
  virtual void registerCallbackForTextMessages(OnTextMessageCallback callback) = 0;

  // Parse message and call registered callback
  virtual void notifyAboutBinaryMessage(PayloadView message) = 0;

  // Parse a text message and call a registered callback.
  // Only one callback is used for all text messages. No message type is needed.
  virtual void notifyAboutTextMessage(std::string_view message) = 0;

  // ---------------------------
  // NEW Interface
  // ---------------------------

  virtual std::vector<std::uint8_t> makeBinaryMessage(PayloadType type, PayloadView payload) const = 0;

  template <class T>
  std::vector<std::uint8_t> makeBinaryMessageMemcpy(PayloadType type, const T& obj) const;

  template <class Callback>
  void parseBinaryMessage(PayloadView message, Callback&& callback) const;
};

// ----------------------------
// Templates implementation
// ----------------------------

template <class T>
std::vector<std::uint8_t> INetworkDataHandler::makeBinaryMessageMemcpy(PayloadType type, const T& obj) const {
  static_assert(std::is_trivially_copyable_v<T>, "makeBinaryMessageMemcpy requires trivially copyable T");
  static_assert(std::is_standard_layout_v<T>, "makeBinaryMessageMemcpy requires standard layout T");

  return makeBinaryMessage(
      type,
      PayloadView{reinterpret_cast<const std::uint8_t*>(&obj), sizeof(T)});
}

// Symmetric encryption for NetworkDataHandler::makeBinaryMessage
template <class Callback>
void INetworkDataHandler::parseBinaryMessage(PayloadView message, Callback&& callback) const {
  if (message.size() < sizeof(PayloadType) + sizeof(std::uint16_t)) {
    throw std::runtime_error("Binary message too small");
  }

  PayloadType type{};
  std::uint16_t payloadSize = 0;

  std::memcpy(&type, message.data(), sizeof(type));
  std::memcpy(&payloadSize, message.data() + sizeof(type), sizeof(payloadSize));

  const std::size_t headerSize = sizeof(type) + sizeof(payloadSize);
  const std::size_t totalSize = headerSize + payloadSize;

  if (message.size() < totalSize) {
    throw std::runtime_error("Binary message payload size mismatch");
  }

  PayloadView payload{message.data() + headerSize, payloadSize};
  callback(type, payload);
}
