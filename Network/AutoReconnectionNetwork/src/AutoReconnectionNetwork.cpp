#include "AutoReconnectionNetwork.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

AutoReconnectionNetwork::~AutoReconnectionNetwork() {
  AutoReconnectionNetwork::stop();
}

void AutoReconnectionNetwork::init(
    std::string_view url,
    OnTextMessageReceived onMessageReceivedCallback,
    OnBinaryMessageReceived onBinaryMessageReceivedCallback,
    StateChangedCallback stateChangedCallback) {
  url_ = url;
  onMessageReceivedCallback_ = onMessageReceivedCallback;
  onBinaryMessageReceivedCallback_ = onBinaryMessageReceivedCallback;
  stateChangedCallback_ = stateChangedCallback;
  doubleQueueNetwork_ = IDoubleQueueNetwork::create();
}

void AutoReconnectionNetwork::start() {
  setStateAndNotify_(State::Disconnected);
  doubleQueueNetwork_->start(url_);
}

void AutoReconnectionNetwork::stop() {
  doubleQueueNetwork_->stop();
}

void AutoReconnectionNetwork::iterate() {
  pollNetworkEvents_();
  if (state_ == State::Connected) {
    drainOutboundEventQueue_();
  }

  // Try to reconnect only when scheduled and avoid spamming start()
  if (state_ == State::Disconnected && reconnectPolicy_.due()) {
    setStateAndNotify_(State::Connecting);
    SPDLOG_INFO("Net: reconnect attempt...");
    doubleQueueNetwork_->start(url_);
  }
}

void AutoReconnectionNetwork::send(std::string_view data) {
  doubleQueueNetwork_->send(data);
}

void AutoReconnectionNetwork::send(std::vector<uint8_t> data) {
  doubleQueueNetwork_->send(data);
}

AutoReconnectionNetwork::State AutoReconnectionNetwork::getState() const {
  return state_;
}

void AutoReconnectionNetwork::setStateAndNotify_(State newState) {
  state_ = newState;
  stateChangedCallback_(newState);
}

// Poll network events (non-blocking)
void AutoReconnectionNetwork::pollNetworkEvents_() {
  IDoubleQueueNetwork::NetEvent ev{};
  int processed = 0;
  constexpr int kMaxEventsPerFrame = 256;
  while (processed < kMaxEventsPerFrame && doubleQueueNetwork_->poll(ev)) {
    ++processed;
    SPDLOG_TRACE("Net: Polling Event type={}", magic_enum::enum_name(ev.type));
    switch (ev.type) {
      case IDoubleQueueNetwork::NetEvent::Type::Connected: {
        std::ostringstream ss;
        SPDLOG_DEBUG("Net: Connected to server at {}", ev.textPayload);
        setStateAndNotify_(State::Connected);
        reconnectPolicy_.onConnected();
        break;
      }
      case IDoubleQueueNetwork::NetEvent::Type::Disconnected: {
        SPDLOG_DEBUG("Net: Disconnected, reason={}", ev.textPayload);
        setStateAndNotify_(State::Disconnected);
        reconnectPolicy_.schedule();
        break;
      }
      case IDoubleQueueNetwork::NetEvent::Type::Error: {
        SPDLOG_DEBUG("Net: Error={}", ev.textPayload);
        setStateAndNotify_(State::Disconnected);
        reconnectPolicy_.schedule();
        break;
      }
      case IDoubleQueueNetwork::NetEvent::Type::TextMessage: {
        SPDLOG_DEBUG("Net: Text={}", ev.textPayload);
        onMessageReceivedCallback_(ev.textPayload);
        break;
      }
      case IDoubleQueueNetwork::NetEvent::Type::BinaryMessage: {
        SPDLOG_DEBUG("Net: Binary size={}", ev.binaryPayload.size());
        onBinaryMessageReceivedCallback_(ev.binaryPayload);
        break;
      }
    }
  }
}

void AutoReconnectionNetwork::drainOutboundEventQueue_() {
  doubleQueueNetwork_->drainOutboundQueue();
}
