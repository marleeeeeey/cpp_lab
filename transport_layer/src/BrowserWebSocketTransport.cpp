#include "BrowserWebSocketTransport.h"

// -------------------------------------
// JavaScript callbacks calls by C++ code
// -------------------------------------

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

EM_JS(void, js_ws_close, (int selfPtr), {
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

// --------------------------------
// Callbacks from JavaScript to C++
// --------------------------------

namespace {

BrowserWebSocketTransport* fromPtr(int selfPtr) {
  return (BrowserWebSocketTransport*)(intptr_t)selfPtr;
}

}  // namespace

extern "C" {
EMSCRIPTEN_KEEPALIVE void ws_on_open(int selfPtr);
EMSCRIPTEN_KEEPALIVE void ws_on_message(int selfPtr, const char* msg);
EMSCRIPTEN_KEEPALIVE void ws_on_close(int selfPtr, int code, const char* reason);
EMSCRIPTEN_KEEPALIVE void ws_on_error(int selfPtr, const char* msg);
}

void ws_on_open(int selfPtr) {
  auto* t = fromPtr(selfPtr);
  if (t->onOpen) t->onOpen();
}

void ws_on_message(int selfPtr, const char* msg) {
  auto* t = fromPtr(selfPtr);
  if (t->onText) t->onText(msg ? msg : "");
}

void ws_on_close(int selfPtr, int code, const char* reason) {
  auto* t = fromPtr(selfPtr);
  if (t->onClose) t->onClose(code, reason ? reason : "");
}

void ws_on_error(int selfPtr, const char* msg) {
  auto* t = fromPtr(selfPtr);
  if (t->onError) t->onError(msg ? msg : "error");
}

// ---------------------------------------------------------
// BrowserWebSocketTransport public interface implementation
// ---------------------------------------------------------

void BrowserWebSocketTransport::connect(std::string url) {
  url_ = std::move(url);
  js_ws_connect((int)(intptr_t)this, url_.c_str());
}

void BrowserWebSocketTransport::sendText(std::string_view text) {
  tmp_.assign(text.begin(), text.end());
  js_ws_send((int)(intptr_t)this, tmp_.c_str());
}

void BrowserWebSocketTransport::close() {
  js_ws_close((int)(intptr_t)this);
}
