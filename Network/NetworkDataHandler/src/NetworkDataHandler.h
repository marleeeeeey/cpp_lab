#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

#include "NetworkDataHandler/INetworkDataHandler.h"

class NetworkDataHandler : public INetworkDataHandler {
 public:
  void registerCallbackForBinaryMessageWithType(MessageType type, OnBinaryMessageCallback callback) override;

  void registerCallbackForTextMessages(OnTextMessageCallback callback) override;

  std::vector<uint8_t> addTypeForBinaryMessage(MessageType type, const std::vector<uint8_t>& payload) override;

  void parseBinaryMessage(const std::vector<uint8_t>& message, OnBinaryMessageCallback callback) override;

  void notifyAboutBinaryMessage(const std::vector<uint8_t>& message) override;

  void notifyAboutTextMessage(std::string_view message) override;

 private:
  std::unordered_map<MessageType, OnBinaryMessageCallback> messageCallbacks_;
  OnTextMessageCallback textMessageCallback_;
};