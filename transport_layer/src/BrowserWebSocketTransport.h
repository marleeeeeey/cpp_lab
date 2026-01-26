#pragma once

#include <emscripten.h>

#include <string>

#include "transport_layer/Transport.h"

class BrowserWebSocketTransport final : public ITransport {
 public:
  void connect(std::string url) override;
  void sendText(std::string_view text) override;
  void close() override;

 private:
  std::string url_;
  std::string tmp_;
};
