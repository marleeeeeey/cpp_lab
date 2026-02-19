#pragma once

#include <App.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string_view>

#include "BinaryMessageParser.h"
#include "NetworkDataHandler/INetworkDataHandler.h"

class ServerState {
 public:
  std::shared_ptr<uWS::App> app;
  std::unique_ptr<INetworkDataHandler> networkDataHandler;
  std::unique_ptr<BinaryMessageParser> binaryMessageParser;
  std::set<std::string> connectedClientNames;

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
