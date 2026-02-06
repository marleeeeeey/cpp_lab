#include <chrono>

struct ReconnectPolicy {
  int attempt = 0;
  std::chrono::steady_clock::time_point next = std::chrono::steady_clock::time_point::min();

  void onConnected();

  void schedule(std::chrono::milliseconds base = std::chrono::milliseconds(250),
                std::chrono::milliseconds cap = std::chrono::seconds(10));

  bool due() const;
};