#pragma once

#include <functional>
#include <memory>
#include <thread>

#include "PerSocketData.h"
#include "ServerState.h"

class ServerGameLoop {
 public:
  ServerGameLoop(std::shared_ptr<ServerState> state, BroadcastCb broadcastCb);
  void start();
  void stop();

 private:
  void updateState_(std::shared_ptr<ServerState> state, float dtSeconds);
  void createWorldSnapshotAndSendToClients_(std::shared_ptr<ServerState> state) const;

  bool stopRequested_ = false;
  std::thread gameThread_;
  std::shared_ptr<ServerState> state_;
  BroadcastCb broadcastCb_;
};
