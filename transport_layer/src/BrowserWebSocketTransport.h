#pragma once

#include <emscripten.h>

#include <string>

#include "transport_layer/Transport.h"

class BrowserWebSocketTransport final : public ITransport {
 public:
  ~BrowserWebSocketTransport() override;

  void connect(std::string url) override;
  void sendText(std::string_view text) override;
  void close() override;

 private:
  std::string url_;
  std::string tmp_;

  void detach() noexcept;
  bool detached_ = false;
};
