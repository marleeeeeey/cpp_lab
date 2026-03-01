#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include "ServerState.h"

class GameLoop {
 public:
  // ---------------------
  // Signatures
  // ---------------------

  using OnBroadcastMessageCallback = std::function<void(MessageType type, const std::vector<uint8_t>&)>;

  // -----------------------
  // Interface
  // -----------------------

  GameLoop(std::shared_ptr<ServerState> state, OnBroadcastMessageCallback onBroadcastMessageCallback);
  void start();
  void stop();

 private:
  void updateState_(std::shared_ptr<ServerState> state);
  void sendStateToClients_(std::shared_ptr<ServerState> state);

  bool stopRequested_ = false;
  std::thread gameThread_;
  std::shared_ptr<ServerState> state_;
  OnBroadcastMessageCallback onBroadcastMessageCallback_;
};
