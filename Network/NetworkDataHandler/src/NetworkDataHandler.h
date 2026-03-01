#pragma once
#include <functional>
#include <unordered_map>
#include <vector>

#include "NetworkDataHandler/INetworkDataHandler.h"

class NetworkDataHandler : public INetworkDataHandler {
 public:
  void registerCallbackForBinaryMessageWithType(PayloadType type, OnBinaryMessageCallback callback) override;

  void registerCallbackForTextMessages(OnTextMessageCallback callback) override;

  void notifyAboutBinaryMessage(PayloadView message) override;

  void notifyAboutTextMessage(std::string_view message) override;

  std::vector<std::uint8_t> makeBinaryMessage(PayloadType type, PayloadView payload) const override;

 private:
  std::unordered_map<PayloadType, OnBinaryMessageCallback> messageCallbacks_;
  OnTextMessageCallback textMessageCallback_;
};