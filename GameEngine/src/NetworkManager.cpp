#include "NetworkManager.h"

#define DEBUG_LOG_DISABLE_DEBUG_LEVEL
#include <magic_enum/magic_enum.hpp>

#include "DebugLog/DebugLog.h"
#include "NetworkTransport/TransportFactory.h"

NetworkManager::NetworkManager(NetworkOptions inOptions) : networkOptions(inOptions) {
}

void NetworkManager::start() {
  debugLog() << "NetworkManager::start() called" << std::endl;
  if (running_.exchange(true)) {
    return;  // already started
  }

  connected_.store(false);

  networkTransport_ = createTransport();
  debugLog() << "Network transport created" << std::endl;

  // ---------------------------------------
  // Initiate connection and message loop
  // ---------------------------------------

  networkTransport_->onOpen = [this]() {
    debugLog() << "Connected to server" << std::endl;
    connected_.store(true);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Connected});
    sendCondVar_.notify_one();  // in case messages are waiting
  };
  networkTransport_->onError = [this](std::string_view errorMsg) {
    connected_.store(false);
    std::cerr << "Failed to connect: " << errorMsg << std::endl;
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Error, std::string(errorMsg)});
  };
  networkTransport_->onText = [this](std::string_view msg) {
    debugLog() << "Recv: " << msg << std::endl;
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::TextMessage, std::string(msg)});
  };
  networkTransport_->onClose = [this](int code, std::string_view reason) {
    connected_.store(false);
    debugLog() << "Connection closed. Code: " << code << ", reason: " << reason << std::endl;
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Disconnected, std::string(reason)});
  };

  // Start a thread to accept incoming messages
  networkTransport_->connect(networkOptions.url);

  if (networkOptions.useOutboundQueue) {
    // Start a sender thread. Otherwise, it will use transport layer functionality
    sendThread_ = std::thread([this]() { sendLoop_(); });
  }
}

void NetworkManager::stop() {
  debugLog() << "NetworkManager::stop() called" << std::endl;
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
  debugLog() << "NetworkManager::send() called with message: " << msg << std::endl;
  if (!networkTransport_) {
    std::cerr << "Network transport isn't initialized!" << std::endl;
  }

  if (!networkOptions.useOutboundQueue) {
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
        std::cerr << "Unexpected event type in outbound queue: " << magic_enum::enum_name(event.type) << std::endl;
      } else {
        networkTransport_->sendText(event.payload);
      }
    }
    if (drained > 0) {
      debugLog() << "NetworkManager::sendLoop_() sent " << drained << " messages" << std::endl;
    }
  }
}