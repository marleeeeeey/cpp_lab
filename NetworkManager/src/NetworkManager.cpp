#include "NetworkManager.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>
#include <mutex>

#include "CanUseThreads.h"
#include "NetworkTransport/TransportFactory.h"

NetworkManager::NetworkManager(NetworkOptions inOptions) : networkOptions_(inOptions) {
  // Block using threads if not supported by browser
  bool oldUseThreads = networkOptions_.useOutboundQueue;
  networkOptions_.useOutboundQueue = canUseThreads() && networkOptions_.useOutboundQueue;

  if (oldUseThreads == true && networkOptions_.useOutboundQueue == false) {
    SPDLOG_WARN(
        "Using threads is not supported by browser. "
        "Falling back to synchronous mode. "
        "Outgoing queue will be disabled.");
  }
}

void NetworkManager::start(std::string_view url) {
  SPDLOG_TRACE("NetworkManager::start");
  if (running_.exchange(true) && connected_.load() == true) {
    return;  // already started
  }

  connected_.store(false);

  networkTransport_ = createTransport();
  SPDLOG_TRACE("Network transport created");

  // ---------------------------------------
  // Initiate connection and message loop
  // ---------------------------------------

  networkTransport_->onOpen = [this]() {
    SPDLOG_TRACE("Connected to server");
    connected_.store(true);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Connected});
    sendCondVar_.notify_one();  // in case messages are waiting
  };
  networkTransport_->onError = [this](std::string_view errorMsg) {
    connected_.store(false);
    SPDLOG_ERROR("Network error: {}", errorMsg);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Error, std::string(errorMsg)});
  };
  networkTransport_->onText = [this](std::string_view msg) {
    SPDLOG_TRACE("Recv: {}", msg);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::TextMessage, std::string(msg)});
  };
  networkTransport_->onClose = [this](int code, std::string_view reason) {
    connected_.store(false);
    SPDLOG_TRACE("Connection closed. Code={}, Reason={}", code, reason);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Disconnected, std::string(reason)});
  };

  // Start a thread to accept incoming messages
  networkTransport_->connect(url);

  if (networkOptions_.useOutboundQueue) {
    // Start a sender thread. Otherwise, it will use transport layer functionality
    sendThread_ = std::thread([this]() { sendLoop_(); });
  }
}

void NetworkManager::stop() {
  SPDLOG_TRACE("NetworkManager::stop");
  if (!running_.exchange(false)) {
    return;
  }

  sendCondVar_.notify_all();

  if (sendThread_.joinable()) {
    sendThread_.join();
  }

  if (networkTransport_) {
    networkTransport_->close();
  }
  networkTransport_.reset();
}

bool NetworkManager::poll(NetEvent& out) {
  return inboundEventQueue_.try_dequeue(out);
}

void NetworkManager::send(std::string msg) {
  SPDLOG_TRACE("NetworkManager::send. msg={}", msg);
  if (!networkTransport_) {
    SPDLOG_ERROR("Network transport isn't initialized");
  }

  if (!networkOptions_.useOutboundQueue) {
    // Without a queue there is no guaranty that "send before connect" will be successful.
    // Set networkOptions_.useOutgoingQueue to true to enable this guaranty.
    networkTransport_->sendText(msg);
  } else {
    outboundEventQueue_.enqueue(NetEvent{NetEvent::Type::TextMessage, msg});
    sendCondVar_.notify_one();
  }
}

// -----------------------
// Sender thread and queue
// -----------------------

void NetworkManager::sendLoop_() {
  while (running_) {
    // Sleep 10ms to reduce CPU usage. Wake up on "notify" (e.g., new outgoing message)
    std::unique_lock sendLock(sendMutex_);
    sendCondVar_.wait_for(sendLock, std::chrono::milliseconds(10));
    sendLock.unlock();

    if (!networkTransport_) {
      continue;
    }
    if (!connected_.load()) {
      continue;  // don't send a message until the transport is actually connected
    }

    NetEvent event;
    int drained = 0;
    constexpr int kMaxSendsPerTick = 1024;
    while (drained < kMaxSendsPerTick && outboundEventQueue_.try_dequeue(event)) {
      ++drained;
      if (event.type != NetEvent::Type::TextMessage) {
        SPDLOG_WARN("Unexpected event type in outbound queue: {}", magic_enum::enum_name(event.type));
      } else {
        networkTransport_->sendText(event.payload);
      }
    }
    if (drained > 0) {
      SPDLOG_TRACE("NetworkManager::sendLoop_() sent {} messages", drained);
    }
  }
}