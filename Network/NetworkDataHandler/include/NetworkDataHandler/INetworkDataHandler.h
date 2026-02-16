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
  virtual std::vector<uint8_t> prepareBinaryMessage(MessageType type, const std::vector<uint8_t>& payload) = 0;

  // Parse message and call registered callback
  virtual void parseBinaryMessage(const std::vector<uint8_t>& message) = 0;

  // Parse text message. No MessageType required
  virtual void parseTextMessage(std::string_view message) = 0;
};