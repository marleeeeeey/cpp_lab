#pragma once
#include <functional>
#include <string_view>

class IAutoReconnectionNetwork {
 public:
  using OnMessageReceived = std::function<void(std::string_view)>;

  enum class State { Disconnected,
                     Connecting,
                     Connected };

  using StateChangedCallback = std::function<void(State)>;

  // Gracefully stop connection on destruction
  virtual ~IAutoReconnectionNetwork() = default;

  // Replacement for the constructor
  virtual void init(std::string_view url,
                    OnMessageReceived onMessageReceivedCallback,
                    StateChangedCallback stateChangedCallback) = 0;

  // Try to connect till the "stop" method will be called
  virtual void start() = 0;

  // Gracefully stop a connection and reconnection process
  virtual void stop() = 0;

  // Should be periodically called to poll/drain network events
  virtual void iterate() = 0;

  // Send data
  virtual void send(std::string_view data) = 0;

  // Get current state
  virtual State getState() const = 0;
};