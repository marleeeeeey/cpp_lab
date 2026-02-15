#pragma once

#include <DoubleQueueNetwork/IDoubleQueueNetwork.h>
#include <concurrentqueue.h>

#include <condition_variable>
#include <memory>
#include <string>

// ---------------------
// Forward declarations
// ---------------------

class ITransport;

// ------------------------
// DoubleQueueNetwork
// ------------------------

// DoubleQueueNetwork adds inbound and (optionally) outbound queues on top of the underlying transport layer.
class DoubleQueueNetwork : public IDoubleQueueNetwork {
 public:
  DoubleQueueNetwork();

 private:
  // -----------------------------
  // DoubleQueueNetwork interface
  // -----------------------------

 public:
  void start(std::string_view url) override;
  void stop() override;
  bool poll(NetEvent& out) override;
  void send(std::string_view msg) override;
  void send(std::vector<uint8_t> payload) override;
  void drainOutboundQueue() override;

 private:
  // -------------------------------------------
  // Network transport and incoming events queue
  // -------------------------------------------

  std::atomic_bool running_{false};
  std::atomic_bool connected_{false};
  std::shared_ptr<ITransport> networkTransport_;
  moodycamel::ConcurrentQueue<NetEvent> inboundEventQueue_;

  // ----------------------------------
  // Sender queue
  // ----------------------------------

  moodycamel::ConcurrentQueue<NetEvent> outboundEventQueue_;
};