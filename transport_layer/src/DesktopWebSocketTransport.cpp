#include "DesktopWebSocketTransport.h"

#define DEBUG_LOG_DISABLE_DEBUG_LEVEL
#define DEBUG_LOG_USER_PREFIX "[DesktopWebSocketTransport]"
#include <debug_log/DebugLog.h>
#include <ixwebsocket/IXNetSystem.h>

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
        debugLog() << "Connection established" << std::endl;
        if (onOpen) onOpen();
        break;
      }
      case ix::WebSocketMessageType::Message: {
        debugLog() << "received message: " << msg->str << std::endl;
        if (onText) onText(msg->str);
        break;
      }
      case ix::WebSocketMessageType::Close: {
        debugLog() << "Connection closed. Code: " << msg->closeInfo.code
                   << ", reason: " << msg->closeInfo.reason << std::endl;
        const int code = msg->closeInfo.code;
        const std::string& reason = msg->closeInfo.reason;
        if (onClose) onClose(code, reason);
        break;
      }
      case ix::WebSocketMessageType::Error: {
        debugLog() << "Connection error: " << msg->errorInfo.reason << std::endl;
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
  debugLog() << "Destroyed" << std::endl;
}

void DesktopWebSocketTransport::connect(std::string url) {
  std::lock_guard<std::mutex> lock(mu_);
  url_ = std::move(url);

  ws_.setUrl(url_);

  if (!started_.exchange(true)) {
    debugLog() << "Connecting to " << url_ << std::endl;
    ws_.start();
  } else {
    debugLog() << "Restarting connection" << std::endl;
    ws_.stop();
    ws_.start();
  }
}

void DesktopWebSocketTransport::sendText(std::string_view text) {
  // ixwebsocket copies data internally; safe to pass a temporary std::string.
  debugLog() << "sendText: " << text << std::endl;
  ws_.sendText(std::string(text));
}

void DesktopWebSocketTransport::close() {
  const bool wasStarted = started_.exchange(false);
  if (wasStarted) {
    debugLog() << "Closing connection" << std::endl;
    ws_.stop();
  }
}
