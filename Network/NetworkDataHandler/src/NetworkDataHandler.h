#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

#include "NetworkDataHandler/INetworkDataHandler.h"

class NetworkDataHandler : public INetworkDataHandler {
 public:
  void registerMessageType(MessageType type, OnMessageCallback callback) override;

  std::vector<uint8_t> prepareMessage(MessageType type, const std::vector<uint8_t>& payload) override;

  void parseMessage(const std::vector<uint8_t>& message) override;

 private:
  std::unordered_map<MessageType, OnMessageCallback> messageCallbacks_;
};