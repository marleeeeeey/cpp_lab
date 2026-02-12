#include "NetworkManager.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

#include "NetworkTransport/TransportFactory.h"

NetworkManager::NetworkManager() {
  networkTransport_ = createTransport();
  SPDLOG_TRACE("Network transport created");

  // -------------------------------------
  // Setup WebSocket transport callbacks
  // -------------------------------------

  networkTransport_->onOpen = [this]() {
    SPDLOG_TRACE("Connected to server");
    connected_.store(true);
    inboundEventQueue_.enqueue(NetEvent{NetEvent::Type::Connected});
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
}

void NetworkManager::start(std::string_view url) {
  SPDLOG_TRACE("NetworkManager::start");
  if (running_.exchange(true) && connected_.load() == true) {
    return;  // already started
  }

  connected_.store(false);

  // Start a thread to accept incoming messages
  networkTransport_->connect(url);
}

void NetworkManager::stop() {
  SPDLOG_TRACE("NetworkManager::stop");
  if (!running_.exchange(false)) {
    return;
  }

  if (networkTransport_) {
    networkTransport_->close();  // blocking
  }
  networkTransport_.reset();
}

bool NetworkManager::poll(NetEvent& out) {
  return inboundEventQueue_.try_dequeue(out);
}

void NetworkManager::send(std::string_view msg) {
  SPDLOG_TRACE("NetworkManager::send. msg={}", msg);
  if (!networkTransport_) {
    SPDLOG_ERROR("Network transport isn't initialized");
  }

  outboundEventQueue_.enqueue(NetEvent{NetEvent::Type::TextMessage, std::string(msg)});
}

void NetworkManager::drainOutboundQueue() {
  if (!networkTransport_) {
    SPDLOG_ERROR("Network transport isn't initialized");
    return;
  }

  // Set queue processing limit
  constexpr int kMaxMessagesPerDrain = 512;

  int processed = 0;
  while (processed < kMaxMessagesPerDrain) {
    NetEvent ev;
    if (!outboundEventQueue_.try_dequeue(ev)) {
      break;
    }

    if (ev.type != NetEvent::Type::TextMessage) {
      SPDLOG_WARN("Unexpected event type: {}. Drop this message", magic_enum::enum_name(ev.type));
      outboundEventQueue_.enqueue(std::move(ev));
      break;
    }

    // Try to send a message
    const auto res = networkTransport_->sendText(ev.payload);
    if (res != ITransport::SendResult::Success) {
      SPDLOG_WARN("Failed to send message. Type: {}, Payload: {}. Message back to the queue",
                  magic_enum::enum_name(ev.type), ev.payload);
      outboundEventQueue_.enqueue(std::move(ev));
      break;
    }

    ++processed;
  }
}
