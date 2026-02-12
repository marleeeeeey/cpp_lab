#include "ReconnectPolicy.h"

#include <algorithm>
#include <random>

void ReconnectPolicy::onConnected() {
  attempt_ = 0;
  nextTimePoint_ = std::chrono::steady_clock::time_point::min();
}

void ReconnectPolicy::schedule(std::chrono::milliseconds base, std::chrono::milliseconds maxDelay) {
  using namespace std::chrono;

  const int a = std::min(attempt_, 10);
  auto delay = base * (1 << a);
  delay = std::min(delay, maxDelay);

  // jitter 0.8..1.2
  static std::mt19937 rng{std::random_device{}()};
  std::uniform_real_distribution<double> dist(0.8, 1.2);
  delay = milliseconds((long long)(delay.count() * dist(rng)));

  nextTimePoint_ = steady_clock::now() + delay;
  ++attempt_;
}

bool ReconnectPolicy::due() const {
  return nextTimePoint_ != std::chrono::steady_clock::time_point::min() &&
         std::chrono::steady_clock::now() >= nextTimePoint_;
}