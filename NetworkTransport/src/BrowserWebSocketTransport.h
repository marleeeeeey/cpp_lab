#pragma once

#include <emscripten.h>

#include <string>

#include "NetworkTransport/Transport.h"

class BrowserWebSocketTransport final : public ITransport {
 public:
  ~BrowserWebSocketTransport() override;

  void connect(std::string_view url) override;
  SendResult sendText(std::string_view text) override;
  void close() override;

 private:
  std::string url_;
  std::string tmp_;

  void detach() noexcept;
  bool detached_ = false;
};
