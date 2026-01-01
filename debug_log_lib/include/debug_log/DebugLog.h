#pragma once

#include <iostream>
#include <thread>

// Simple logging function. Logs thread id before each line.
// To enable/disable logging, use `DISABLE_DEBUG_LOG` macro.
// USAGE:
// debugLog() << "line" << msg << std::endl;

inline std::ostream& getDebugLogStream() {
  std::cout << "[" << std::this_thread::get_id() << "] ";
  return std::cout;
}

struct NullStream {
  template <typename T>
  NullStream& operator<<(const T&) { return *this; }

  NullStream& operator<<(std::ostream& (*)(std::ostream&)) { return *this; }
};

#ifdef DISABLE_DEBUG_LOG
#define debugLog() \
  if (true) {      \
  } else           \
    NullStream()
#else
#define debugLog() getDebugLogStream()
#endif