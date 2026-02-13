#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

class INetworkDataHandler {
 public:
  // Factory method to create an instance
  static std::unique_ptr<INetworkDataHandler> create();

  // Type signature
  using MessageType = uint16_t;

  // Callback signature
  using OnMessageCallback = std::function<void(const MessageType type, const std::vector<uint8_t>& payload)>;

  // Register a callback for a specific message type
  virtual void registerMessageType(MessageType type, OnMessageCallback callback) = 0;

  // Prepare a message for sending
  virtual std::vector<uint8_t> prepareMessage(MessageType type, const std::vector<uint8_t>& payload) = 0;

  // Parse message and call registered callback
  virtual void parseMessage(const std::vector<uint8_t>& message) = 0;
};