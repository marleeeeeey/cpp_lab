#pragma once

#include <App.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string_view>

#include "NetworkDataHandler/INetworkDataHandler.h"

class ServerState {
 public:
  std::shared_ptr<uWS::TemplatedApp<false>> app;
  std::unique_ptr<INetworkDataHandler> networkDataHandler;

  ServerState();
  void incrementNumberOfClients();
  void decrementNumberOfClients();
  const std::string_view& getBroadcastTopicName() const;

 private:
  const std::string_view broadcastTopicName_ = "broadcast";
  int numberOfClients_ = 0;
  int maxNumberOfClients_ = 0;

  void broadcastNumberClients_();
};
