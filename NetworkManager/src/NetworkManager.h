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
  // -----------------------------
  // Class options and constructor
  // -----------------------------

 public:
  struct NetworkOptions {
    // useOutboundQueue:
    // Probably this queue already implemented in the transport layer and may be disabled.
    // If "false", there is no guaranty that "send before connect" will be successful.
    bool useOutboundQueue = true;
  };

  NetworkManager(NetworkOptions inOptions);

 private:
  NetworkOptions networkOptions_;

  // -----------------------------
  // NetworkManager interface
  // -----------------------------

 public:
  void start(std::string_view url);  // Starts network thread (non-blocking)
  void stop();                       // Correctly terminates (blocking)
  bool poll(NetEvent& out);          // try-pop one-event (non-blocking)
  void send(std::string msg);        // Sends message (non-blocking)

 private:
  // -------------------------------------------
  // Network transport and incoming events queue
  // -------------------------------------------

  std::atomic_bool running_{false};
  std::atomic_bool connected_{false};
  std::shared_ptr<ITransport> networkTransport_;
  moodycamel::ConcurrentQueue<NetEvent> inboundEventQueue_;

  // ----------------------------------
  // Sender thread and queue (optional)
  // ----------------------------------

  moodycamel::ConcurrentQueue<NetEvent> outboundEventQueue_;
  std::thread sendThread_;
  std::mutex sendMutex_;
  std::condition_variable sendCondVar_;
  void sendLoop_();
};