#include "NetworkTransport/INetworkTransport.h"

#ifdef __EMSCRIPTEN__
#include <BrowserWebSocketTransport.h>
#else
#include <DesktopWebSocketTransport.h>
#endif

std::unique_ptr<INetworkTransport> INetworkTransport::create() {
#ifdef __EMSCRIPTEN__
  return std::make_unique<BrowserWebSocketTransport>();
#else
  return std::make_unique<DesktopWebSocketTransport>();
#endif
}