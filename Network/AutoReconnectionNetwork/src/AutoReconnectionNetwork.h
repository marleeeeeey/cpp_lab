#pragma once

#include <functional>

#include "AutoReconnectionNetwork/IAutoReconnectionNetwork.h"
#include "DoubleQueueNetwork/IDoubleQueueNetwork.h"
#include "ReconnectPolicy.h"

class AutoReconnectionNetwork : public IAutoReconnectionNetwork {
 public:
  ~AutoReconnectionNetwork() override;
  void init(std::string_view url,
            OnTextMessageReceived onMessageReceivedCallback,
            OnBinaryMessageReceived onBinaryMessageReceivedCallback,
            StateChangedCallback stateChangedCallback) override;
  void start() override;
  void stop() override;
  void iterate() override;
  void send(std::string_view data) override;
  void send(std::vector<uint8_t> data) override;
  State getState() const override;

 private:
  std::string url_;
  OnTextMessageReceived onMessageReceivedCallback_;
  OnBinaryMessageReceived onBinaryMessageReceivedCallback_;
  StateChangedCallback stateChangedCallback_;
  ReconnectPolicy reconnectPolicy_;
  State state_ = State::Disconnected;

  void setStateAndNotify_(State newState);
  void pollNetworkEvents_();
  void drainOutboundEventQueue_();
  std::unique_ptr<IDoubleQueueNetwork> doubleQueueNetwork_;
};
