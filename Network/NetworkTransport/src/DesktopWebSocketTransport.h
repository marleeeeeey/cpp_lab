#pragma once
#include <ixwebsocket/IXWebSocket.h>

#include <atomic>
#include <mutex>
#include <string>

#include "NetworkTransport/Transport.h"

class DesktopWebSocketTransport final : public ITransport {
 public:
  DesktopWebSocketTransport();
  ~DesktopWebSocketTransport() override;

  void connect(std::string_view url) override;
  ITransport::SendResult sendText(std::string_view text) override;
  void close() override;

 private:
  // Global init/deinit of ixwebsocket
  static void retainNet();
  static void releaseNet();
  static std::atomic<int> netRefCount_;

  // State
  ix::WebSocket ws_;
  std::mutex mu_;
  std::string url_;
  std::atomic<bool> started_{false};
};