#include "CanUseThreads.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/threading.h>
#endif

bool canUseThreads() {
#ifdef __EMSCRIPTEN__
  return emscripten_has_threading_support();
#else
  return true;
#endif
}