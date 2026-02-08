#pragma once

#include <NetworkManager/INetworkManager.h>
#include <concurrentqueue.h>

#include <condition_variable>
#include <memory>
#include <string>

// ---------------------
// Forward declarations
// ---------------------

class ITransport;

// ------------------------
// NetworkManager
// ------------------------

// NetworkManager adds inbound and (optionally) outbound queues on top of the underlying transport layer.
class NetworkManager : public INetworkManager {
 public:
  NetworkManager();

 private:
  // -----------------------------
  // NetworkManager interface
  // -----------------------------

 public:
  void start(std::string_view url) override;
  void stop() override;
  bool poll(NetEvent& out) override;
  void send(std::string msg) override;
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