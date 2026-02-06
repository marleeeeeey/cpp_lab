#include "ReconnectPolicy.h"

#include <algorithm>
#include <random>

void ReconnectPolicy::onConnected() {
  attempt = 0;
  next = std::chrono::steady_clock::time_point::min();
}

void ReconnectPolicy::schedule(std::chrono::milliseconds base, std::chrono::milliseconds cap) {
  using namespace std::chrono;

  const int a = std::min(attempt, 10);
  auto delay = base * (1 << a);
  delay = std::min(delay, cap);

  // jitter 0.8..1.2
  static std::mt19937 rng{std::random_device{}()};
  std::uniform_real_distribution<double> dist(0.8, 1.2);
  delay = milliseconds((long long)(delay.count() * dist(rng)));

  next = steady_clock::now() + delay;
  ++attempt;
}

bool ReconnectPolicy::due() const {
  return next != std::chrono::steady_clock::time_point::min() && std::chrono::steady_clock::now() >= next;
}