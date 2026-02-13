#include "AutoReconnectionNetwork.h"

#include <spdlog/spdlog.h>

#include "DoubleQueueNetwork/DoubleQueueNetworkFactory.h"

AutoReconnectionNetwork::~AutoReconnectionNetwork() {
  AutoReconnectionNetwork::stop();
}

void AutoReconnectionNetwork::init(std::string_view url, OnMessageReceived onMessageReceivedCallback, StateChangedCallback stateChangedCallback) {
  url_ = url;
  onMessageReceivedCallback_ = onMessageReceivedCallback;
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
  drainOutboundEventQueue_();

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
    switch (ev.type) {
      case NetEvent::Type::Connected: {
        std::ostringstream ss;
        SPDLOG_DEBUG("Net: Connected to server at {}", ev.payload);
        setStateAndNotify_(State::Connected);
        reconnectPolicy_.onConnected();
        break;
      }
      case NetEvent::Type::Disconnected: {
        SPDLOG_DEBUG("Net: Disconnected, reason={}", ev.payload);
        setStateAndNotify_(State::Disconnected);
        reconnectPolicy_.schedule();
        break;
      }
      case NetEvent::Type::Error: {
        SPDLOG_DEBUG("Net: Error={}", ev.payload);
        setStateAndNotify_(State::Disconnected);
        reconnectPolicy_.schedule();
        break;
      }
      case NetEvent::Type::TextMessage: {
        SPDLOG_DEBUG("Net: Text={}", ev.payload);
        onMessageReceivedCallback_(ev.payload);
        break;
      }
    }
  }
}

void AutoReconnectionNetwork::drainOutboundEventQueue_() {
  networkManager_->drainOutboundQueue();
}
