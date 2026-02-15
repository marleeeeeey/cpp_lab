#pragma once

#include <emscripten.h>

#include <string>

#include "NetworkTransport/INetworkTransport.h"

class BrowserWebSocketTransport final : public INetworkTransport {
 public:
  ~BrowserWebSocketTransport() override;

  void connect(std::string_view url) override;
  SendResult sendText(std::string_view text) override;
  SendResult sendBinary(const std::vector<uint8_t>& data) override;
  void close() override;

 private:
  std::string url_;
  std::string tmp_;

  void detach() noexcept;
  bool detached_ = false;
};
