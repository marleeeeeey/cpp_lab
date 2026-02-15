#include "AutoReconnectionNetwork.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>

#include "DoubleQueueNetwork/DoubleQueueNetworkFactory.h"

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
  networkManager_ = DoubleQueueNetworkFactory::createDoubleQueueNetwork();
}

void AutoReconnectionNetwork::start() {
  setStateAndNotify_(State::Disconnected);
  networkManager_->start(url_);
}

void AutoReconnectionNetwork::stop() {
  networkManager_->stop();
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
    networkManager_->start(url_);
  }
}

void AutoReconnectionNetwork::send(std::string_view data) {
  networkManager_->send(data);
}

void AutoReconnectionNetwork::send(std::vector<uint8_t> data) {
  networkManager_->send(data);
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
  NetEvent ev{};
  int processed = 0;
  constexpr int kMaxEventsPerFrame = 256;
  while (processed < kMaxEventsPerFrame && networkManager_->poll(ev)) {
    ++processed;
    SPDLOG_TRACE("Net: Polling Event type={}", magic_enum::enum_name(ev.type));
    switch (ev.type) {
      case NetEvent::Type::Connected: {
        std::ostringstream ss;
        SPDLOG_DEBUG("Net: Connected to server at {}", ev.textPayload);
        setStateAndNotify_(State::Connected);
        reconnectPolicy_.onConnected();
        break;
      }
      case NetEvent::Type::Disconnected: {
        SPDLOG_DEBUG("Net: Disconnected, reason={}", ev.textPayload);
        setStateAndNotify_(State::Disconnected);
        reconnectPolicy_.schedule();
        break;
      }
      case NetEvent::Type::Error: {
        SPDLOG_DEBUG("Net: Error={}", ev.textPayload);
        setStateAndNotify_(State::Disconnected);
        reconnectPolicy_.schedule();
        break;
      }
      case NetEvent::Type::TextMessage: {
        SPDLOG_DEBUG("Net: Text={}", ev.textPayload);
        onMessageReceivedCallback_(ev.textPayload);
        break;
      }
      case NetEvent::Type::BinaryMessage: {
        SPDLOG_DEBUG("Net: Binary size={}", ev.binaryPayload.size());
        onBinaryMessageReceivedCallback_(ev.binaryPayload);
        break;
      }
    }
  }
}

void AutoReconnectionNetwork::drainOutboundEventQueue_() {
  networkManager_->drainOutboundQueue();
}
