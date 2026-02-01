#pragma once

#include <string>

// ------------------------
// NetEvent
// ------------------------

struct NetEvent {
  enum class Type { Connected,
                    Disconnected,
                    TextMessage,
                    Error };
  Type type;
  std::string payload;
};

// ------------------------
// INetworkManager
// ------------------------

// INetworkManager adds poll method, inbound and (optionally) outbound queues
// on top of the underlying transport layer.
class INetworkManager {
 public:
  virtual ~INetworkManager() = default;
  virtual void start(std::string_view url) = 0;  // Starts network thread (non-blocking)
  virtual void stop() = 0;                       // Correctly terminates (blocking)
  virtual bool poll(NetEvent& out) = 0;          // try-pop one-event (non-blocking)
  virtual void send(std::string msg) = 0;        // Sends message (non-blocking)
};
