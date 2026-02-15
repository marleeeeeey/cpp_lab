#pragma once
#include <ixwebsocket/IXWebSocket.h>

#include <atomic>
#include <mutex>
#include <string>

#include "NetworkTransport/INetworkTransport.h"

class DesktopWebSocketTransport final : public INetworkTransport {
 public:
  DesktopWebSocketTransport();
  ~DesktopWebSocketTransport() override;

  void connect(std::string_view url) override;
  SendResult sendText(std::string_view text) override;
  SendResult sendBinary(const std::vector<uint8_t>& data) override;
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