#include "DesktopWebSocketTransport.h"

#include <ixwebsocket/IXNetSystem.h>
#include <spdlog/spdlog.h>

#include <utility>

std::atomic<int> DesktopWebSocketTransport::netRefCount_{0};

void DesktopWebSocketTransport::retainNet() {
  if (netRefCount_.fetch_add(1) == 0) {
    ix::initNetSystem();
  }
}

void DesktopWebSocketTransport::releaseNet() {
  if (netRefCount_.fetch_sub(1) == 1) {
    ix::uninitNetSystem();
  }
}

DesktopWebSocketTransport::DesktopWebSocketTransport() {
  retainNet();

  // Forward ixwebsocket events to ITransport callbacks
  ws_.setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
    if (!msg) return;

    switch (msg->type) {
      case ix::WebSocketMessageType::Open: {
        SPDLOG_TRACE("ix::WebSocketMessageType::Open");
        if (onOpen) onOpen();
        break;
      }
      case ix::WebSocketMessageType::Message: {
        SPDLOG_TRACE("ix::WebSocketMessageType::Message. IsBinary={}", msg->binary);
        if (msg->binary) {
          if (onBinary) {
            std::vector<uint8_t> binaryData(msg->str.begin(), msg->str.end());
            onBinary(binaryData.data(), binaryData.size());
          }
        } else {
          if (onText) {
            onText(msg->str);
          }
        }

        break;
      }
      case ix::WebSocketMessageType::Close: {
        SPDLOG_TRACE("ix::WebSocketMessageType::Close - Code={}, Reason={}",
                     msg->closeInfo.code, msg->closeInfo.reason);
        const int code = msg->closeInfo.code;
        const std::string& reason = msg->closeInfo.reason;
        if (onClose) onClose(code, reason);
        break;
      }
      case ix::WebSocketMessageType::Error: {
        SPDLOG_ERROR("ix::WebSocketMessageType::Error - {}", msg->errorInfo.reason);
        std::string err = msg->errorInfo.reason;
        if (err.empty()) err = "websocket error";
        if (onError) onError(err);
        break;
      }
      default:
        break;
    }
  });
}

DesktopWebSocketTransport::~DesktopWebSocketTransport() {
  close();
  releaseNet();
  SPDLOG_TRACE("DesktopWebSocketTransport destroyed");
}

// Start or restart a working thread.
void DesktopWebSocketTransport::connect(std::string_view url) {
  std::lock_guard<std::mutex> lock(mu_);
  url_ = std::move(url);

  ws_.setUrl(url_);

  if (!started_.exchange(true)) {
    SPDLOG_TRACE("Starting connection to {}", url_);
    ws_.start();
  } else {
    SPDLOG_TRACE("Restarting connection to {}", url_);
    ws_.stop();
    ws_.start();
  }
}

// Almost always is non-blocking. It may be blocking if you're trying to send
// a message from several threads.
INetworkTransport::SendResult DesktopWebSocketTransport::sendText(std::string_view text) {
  // ixwebsocket copies data internally; safe to pass a temporary std::string.
  SPDLOG_TRACE("sendText: {}", text);
  ix::WebSocketSendInfo result = ws_.sendText(std::string(text));
  if (result.success == false) {
    SPDLOG_WARN("sendText failed");
    return SendResult::Error;
  }
  return SendResult::Success;
}

INetworkTransport::SendResult DesktopWebSocketTransport::sendBinary(const std::vector<uint8_t>& data) {
  // ixwebsocket copies data internally; safe to pass a temporary std::string.
  SPDLOG_TRACE("sendText: size={}", data.size());
  ix::WebSocketSendInfo result = ws_.sendBinary(data);
  if (result.success == false) {
    SPDLOG_WARN("sendText failed");
    return SendResult::Error;
  }
  return SendResult::Success;
}

// This is a blocking method. It waits when the working thread is done.
void DesktopWebSocketTransport::close() {
  const bool wasStarted = started_.exchange(false);
  if (wasStarted) {
    SPDLOG_TRACE("Closing connection to {}", url_);
    ws_.stop();
  }
}
