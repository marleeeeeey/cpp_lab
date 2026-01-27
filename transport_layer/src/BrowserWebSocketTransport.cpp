#include "BrowserWebSocketTransport.h"

#define DEBUG_LOG_DISABLE_DEBUG_LEVEL
#define DEBUG_LOG_USER_PREFIX "[BrowserWebSocketTransport]"
#include <debug_log/DebugLog.h>

#include <unordered_set>

// ------------------------------------------------
// JavaScript code: setup callback for JS WebSocket
// ------------------------------------------------

// clang-format off
namespace {

EM_JS(void, js_ws_connect, (int selfPtr, const char* url), {
  const self = selfPtr;

  if (!Module.__wsMap) Module.__wsMap = new Map();

  const ws = new WebSocket(UTF8ToString(url));
  
  ws.onopen = () => Module._ws_on_open(self);

  ws.onmessage = (e) => {
    const s = (typeof e.data === 'string') ? e.data : ('' + e.data);
    const p = stringToNewUTF8(s);
    Module._ws_on_message(self, p);
    _free(p);
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

EM_JS(void, js_ws_send, (int selfPtr, const char* text), {
  const ws = Module.__wsMap && Module.__wsMap.get(selfPtr);
  if (ws && ws.readyState === WebSocket.OPEN) ws.send(UTF8ToString(text));
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
EMSCRIPTEN_KEEPALIVE void ws_on_close(int selfPtr, int code, const char* reason);
EMSCRIPTEN_KEEPALIVE void ws_on_error(int selfPtr, const char* msg);
}

void ws_on_open(int selfPtr) {
  debugLog() << "ws_on_open" << std::endl;
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onOpen) socket->onOpen();
}

void ws_on_message(int selfPtr, const char* msg) {
  debugLog() << "ws_on_message: " << msg << std::endl;
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onText) socket->onText(msg ? msg : "");
}

void ws_on_close(int selfPtr, int code, const char* reason) {
  debugLog() << "ws_on_close: " << code << ", reason: " << reason << std::endl;
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onClose) socket->onClose(code, reason ? reason : "");
}

void ws_on_error(int selfPtr, const char* msg) {
  debugLog() << "ws_on_error: " << msg << std::endl;
  auto* socket = getSocketFromPtrIfAlive(selfPtr);
  if (!socket) return;
  if (socket->onError) socket->onError(msg ? msg : "error");
}

// ----------------------------------------
// BrowserWebSocketTransport implementation
// ----------------------------------------

BrowserWebSocketTransport::~BrowserWebSocketTransport() {
  detach();
  debugLog() << "Destroyed" << std::endl;
}

void BrowserWebSocketTransport::connect(std::string url) {
  url_ = std::move(url);

  detached_ = false;
  aliveSet().insert((intptr_t)this);

  js_ws_connect((int)(intptr_t)this, url_.c_str());
  debugLog() << "Connecting to " << url_ << std::endl;
}

void BrowserWebSocketTransport::sendText(std::string_view text) {
  tmp_.assign(text.begin(), text.end());
  js_ws_send((int)(intptr_t)this, tmp_.c_str());
  debugLog() << "sendText: " << text << std::endl;
}

// Ask to close. Callbacks are still allowed. Object is alive.
void BrowserWebSocketTransport::close() {
  js_ws_close((int)(intptr_t)this);
  debugLog() << "Closing connection" << std::endl;
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
  debugLog() << "Detached" << std::endl;
}
