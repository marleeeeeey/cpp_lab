#pragma once

#include <cstdint>
#include <string>
#include <vector>

// ------------------------
// NetEvent
// ------------------------

struct NetEvent {
  enum class Type { Connected,
                    Disconnected,
                    TextMessage,
                    BinaryMessage,
                    Error };
  Type type;
  std::string textPayload;
  std::vector<uint8_t> binaryPayload;
};

// ------------------------
// IDoubleQueueNetwork
// ------------------------

// IDoubleQueueNetwork adds poll method, inbound and outbound queues
// on top of the underlying transport layer.
class IDoubleQueueNetwork {
 public:
  virtual ~IDoubleQueueNetwork() = default;

  // Starts network thread (non-blocking)
  virtual void start(std::string_view url) = 0;

  // Correctly terminates (blocking)
  virtual void stop() = 0;

  // try-pop one-event (non-blocking)
  virtual bool poll(NetEvent& out) = 0;

  // Add a message to the queue (non-blocking)
  virtual void send(std::string_view msg) = 0;

  // Add a message to the queue (non-blocking)
  virtual void send(std::vector<uint8_t> payload) = 0;

  // Drain outbound queue (blocking)
  virtual void drainOutboundQueue() = 0;
};
