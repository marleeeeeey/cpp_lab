#include "transport_layer/TransportFactory.h"

#ifdef __EMSCRIPTEN__
#include <BrowserWebSocketTransport.h>
#else
#include <DesktopWebSocketTransport.h>
#endif

std::unique_ptr<ITransport> createTransport() {
#ifdef __EMSCRIPTEN__
  return std::make_unique<BrowserWebSocketTransport>();
#else
  return std::make_unique<DesktopWebSocketTransport>();
#endif
}