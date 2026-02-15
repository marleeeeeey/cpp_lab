#include "BrowserWebSocketTransport.h"

#include <spdlog/spdlog.h>

#include <magic_enum/magic_enum.hpp>
#include <unordered_set>

// ------------------------------------------------
// JavaScript code: setup callback for JS WebSocket
// ------------------------------------------------

// clang-format off
namespace {

EM_JS(void, js_ws_connect, (int selfPtr, const char* url), {
  const self = selfPtr;

  if (!Module.__wsMap) Module.__wsMap = new Map();
  if (!Module.__wsGen) Module.__wsGen = new Map();

  // Bump generation for this C++ object (reconnect safety).
  const gen = (Module.__wsGen.get(self) || 0) + 1;
  Module.__wsGen.set(self, gen);

  // If there is an old socket for this object, detach and close it.
  const old = Module.__wsMap.get(self);
  if (old) {
    try {
      old.onopen = null;
      old.onmessage = null;
      old.onclose = null;
      old.onerror = null;
      old.close();
    } catch (e) {}
    Module.__wsMap.delete(self);
  }

  const ws = new WebSocket(UTF8ToString(url));
  
  ws.onopen = () => Module._ws_on_open(self);

  // ----------------------------------
  // Receive string and binary format
  // ----------------------------------

  ws.binaryType = "arraybuffer";

  ws.onmessage = (e) => {
    if (typeof e.data === 'string') {
      // string
      const p = stringToNewUTF8(e.data);
      Module._ws_on_message_text(self, p);
      _free(p);
    } else {
      // binary
      const bytes = new Uint8Array(e.data);
      const size = bytes.length;
      const ptr = _malloc(size);
      HEAPU8.set(bytes, ptr);
      Module._ws_on_message_binary(self, ptr, size);
      _free(ptr);
    }
  };

  ws.onclose = (e) => {
    const reason = e.reason || '';
    const p = stringToNewUTF8(reason);
    Module._ws_on_close(self, e.code || 1000, p);
    _free(p);
  };

  ws.onerror = () => {
    const p = stringToNewUTF8('websocket error');
    Module._ws_on_error(self, p);
    _free(p);
  };

  Module.__wsMap.set(self, ws);
});

// js_ws_send returns:
// 0 (SUCCESS) - message added to browser queue.
// 1 (ERROR) - socket is not ready.
// 2 (ERROR) - socket is not opened.
EM_JS(int, js_ws_send, (int selfPtr, const char* text), {
  try {
    const ws = Module.__wsMap && Module.__wsMap.get(selfPtr);
    if (!ws) return 1;
    if (ws.readyState !== WebSocket.OPEN) return 2;
    ws.send(UTF8ToString(text));
    return 0; // queued to browser
  } catch (e) {
    return 3;
  }
});

// Send binary data
EM_JS(int, js_ws_send_binary, (int selfPtr, uint8_t* data, int size), {
  try {
    const ws = Module.__wsMap && Module.__wsMap.get(selfPtr);
    if (!ws) return 1;
    if (ws.readyState !== WebSocket.OPEN) return 2;

    const bytes = HEAPU8.slice(data, data + size);
    ws.send(bytes);
    return 0;
  } catch (e) {
    return 3;
  }
});

// close should not destroy the underlying socket.
// Socket is needed to receive callbacks correctly.
// To safely destroy the socket, use js_ws_detach.
EM_JS(void, js_ws_close, (int selfPtr), {
  const ws = Module.__wsMap && Module.__wsMap.get(selfPtr);
  if (ws) {
    try { ws.close(); } catch (e) {}
  }
});

// Safe destruction of the underlying socket.
// Close the socket when all callbacks are detached.
EM_JS(void, js_ws_detach, (int selfPtr), {
  const ws = Module.__wsMap && Module.__wsMap.get(selfPtr);
  if (ws) {
    ws.onopen = null;
    ws.onmessage = null;
    ws.onclose = null;
    ws.onerror = null;

    try { ws.close(); } catch (e) {}
    Module.__wsMap.delete(selfPtr);
  }
});

}  // namespace

// clang-format on

// ---------------------------------
// Global repository of live objects
// ---------------------------------

namespace {
// Repository of live objects to ignore late events for dead objects.
std::unordered_set<intptr_t>& aliveSet() {
  static std::unordered_set<intptr_t> globalAliveSet;
  return globalAliveSet;
}

bool isAlive(int selfPtr) {
  return aliveSet().contains((intptr_t)selfPtr);
}

BrowserWebSocketTransport* getSocketFromPtrIfAlive(int selfPtr) {
  if (!isAlive(selfPtr)) return nullptr;
  return (BrowserWebSocketTransport*)(intptr_t)selfPtr;
}

}  // namespace

// --------------------------------------------
// C++ handlers for JavaScript WebSocket events
// --------------------------------------------

extern "C" {
EMSCRIPTEN_KEEPALIVE void ws_on_open(int selfPtr);
EMSCRIPTEN_KEEPALIVE void ws_on_message(int selfPtr, const char* msg);
EMSCRIPTEN_KEEPALIVE void ws_on_message_binary(int selfPtr, uint8_t* data, int size);
EMSCRIPTEN_KEEPALIVE void ws_on_close(int selfPtr, int code, const char* reason);
EMSCRIPTEN_KEEPALIVE void ws_on_error(int selfPtr, const char* msg);
}

void ws_on_open(int selfPtr) {
  SPDLOG_DEBUG("ws_on_open");
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onOpen) socket->onOpen();
}

void ws_on_message(int selfPtr, const char* msg) {
  SPDLOG_DEBUG("ws_on_message: {}", msg);
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onText) socket->onText(msg ? msg : "");
}

void ws_on_message_binary(int selfPtr, uint8_t* data, int size) {
  SPDLOG_DEBUG("ws_on_message_binary: size={}", size);
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;

  if (socket->onBinary) {
    socket->onBinary(data, size);
  }
}

void ws_on_close(int selfPtr, int code, const char* reason) {
  SPDLOG_DEBUG("ws_on_close: code={}, reason={}", code, reason);
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onClose) socket->onClose(code, reason ? reason : "");
}

void ws_on_error(int selfPtr, const char* msg) {
  SPDLOG_ERROR("ws_on_error: {}", msg);
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onError) socket->onError(msg ? msg : "error");
}

// ----------------------------------------
// BrowserWebSocketTransport implementation
// ----------------------------------------

BrowserWebSocketTransport::~BrowserWebSocketTransport() {
  detach();
  SPDLOG_DEBUG("BrowserWebSocketTransport destroyed");
}

void BrowserWebSocketTransport::connect(std::string_view url) {
  url_ = std::move(url);

  detached_ = false;
  aliveSet().insert((intptr_t)this);

  js_ws_connect((int)(intptr_t)this, url_.c_str());
  SPDLOG_DEBUG("BrowserWebSocketTransport connected to {}", url_);
}

ITransport::SendResult BrowserWebSocketTransport::sendText(std::string_view text) {
  tmp_.assign(text.begin(), text.end());
  auto result = (ITransport::SendResult)js_ws_send((int)(intptr_t)this, tmp_.c_str());
  SPDLOG_TRACE("BrowserWebSocketTransport sendText: {}", text);
  if (result != ITransport::SendResult::Success) {
    SPDLOG_WARN("BrowserWebSocketTransport sendText failed with error: {}", magic_enum::enum_name(result));
    return ITransport::SendResult::Error;
  }
  return ITransport::SendResult::Success;
}

ITransport::SendResult BrowserWebSocketTransport::sendBinary(const std::vector<uint8_t>& data) {
  auto result = (ITransport::SendResult)js_ws_send_binary((int)(intptr_t)this, (uint8_t*)data.data(), data.size());
  SPDLOG_TRACE("BrowserWebSocketTransport sendBinary: size={}", data.size());
  if (result != ITransport::SendResult::Success) {
    SPDLOG_WARN("BrowserWebSocketTransport sendBinary failed with error: {}", magic_enum::enum_name(result));
    return ITransport::SendResult::Error;
  }
  return ITransport::SendResult::Success;
}

// Ask to close. Callbacks are still allowed. Object is alive.
void BrowserWebSocketTransport::close() {
  js_ws_close((int)(intptr_t)this);
  SPDLOG_TRACE("BrowserWebSocketTransport closed");
}

// Remove callback and socket map entry (this pointer) in JS.
// Also, reset C++ callbacks to prevent incorrect usage of lambda captures from a client.
// It solves the problem of an immortal object if it captures this pointer in lambda.
void BrowserWebSocketTransport::detach() noexcept {
  if (detached_) return;
  detached_ = true;

  aliveSet().erase((intptr_t)this);

  onOpen = nullptr;
  onText = nullptr;
  onClose = nullptr;
  onError = nullptr;

  js_ws_detach((int)(intptr_t)this);
  SPDLOG_DEBUG("BrowserWebSocketTransport detached");
}
