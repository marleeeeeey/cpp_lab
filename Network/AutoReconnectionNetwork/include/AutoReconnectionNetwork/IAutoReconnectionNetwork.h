#pragma once
#include <cstdint>
#include <functional>
#include <string_view>

class IAutoReconnectionNetwork {
 public:
  using OnTextMessageReceived = std::function<void(std::string_view)>;
  using OnBinaryMessageReceived = std::function<void(std::vector<uint8_t>)>;  // TODO: use span here?

  enum class State { Disconnected,
                     Connecting,
                     Connected };

  using StateChangedCallback = std::function<void(State)>;

  // Gracefully stop connection on destruction
  virtual ~IAutoReconnectionNetwork() = default;

  // Replacement for the constructor
  virtual void init(std::string_view url,
                    OnTextMessageReceived onMessageReceivedCallback,
                    OnBinaryMessageReceived onBinaryMessageReceivedCallback,
                    StateChangedCallback stateChangedCallback) = 0;

  // Try to connect till the "stop" method will be called
  virtual void start() = 0;

  // Gracefully stop a connection and reconnection process
  virtual void stop() = 0;

  // Should be periodically called to poll/drain network events
  virtual void iterate() = 0;

  // Send data
  virtual void send(std::string_view data) = 0;  // TODO: rename to sendText

  // Send data
  virtual void send(std::vector<uint8_t> data) = 0;  // TODO: rename to sendBinary

  // Get current state
  virtual State getState() const = 0;
};