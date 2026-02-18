#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string_view>
#include <vector>

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

  // Type signature
  using MessageType = uint16_t;

  // Callback signature
  using OnBinaryMessageCallback = std::function<void(const MessageType type, const std::vector<uint8_t>& payload)>;
  using OnTextMessageCallback = std::function<void(const std::string_view message)>;

  // ------------
  // Interface
  // ------------

  // Register a callback for a binary with specific message type
  virtual void registerCallbackForBinaryMessageWithType(MessageType type, OnBinaryMessageCallback callback) = 0;

  // Register union callback for all text messages (obsolete method)
  virtual void registerCallbackForTextMessages(OnTextMessageCallback callback) = 0;

  // Prepare a message for sending
  virtual std::vector<uint8_t> addTypeForBinaryMessage(MessageType type, const std::vector<uint8_t>& payload) = 0;

  // Parse message and call mentioned callback. May be used in-place.
  // Callback registration is not needed.
  virtual void parseBinaryMessage(const std::vector<uint8_t>& message, OnBinaryMessageCallback callback) = 0;

  // Parse message and call registered callback
  virtual void notifyAboutBinaryMessage(const std::vector<uint8_t>& message) = 0;

  // Parse a text message and call a registered callback.
  // Only one callback is used for all text messages. No message type is needed.
  virtual void notifyAboutTextMessage(std::string_view message) = 0;
};