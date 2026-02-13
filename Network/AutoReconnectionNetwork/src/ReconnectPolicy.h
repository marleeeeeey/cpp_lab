#pragma once
#include <chrono>

class ReconnectPolicy {
  int attempt_ = 0;
  std::chrono::steady_clock::time_point nextTimePoint_ = std::chrono::steady_clock::time_point::min();

 public:
  void onConnected();

  void schedule(std::chrono::milliseconds base = std::chrono::milliseconds(250),
                std::chrono::milliseconds maxDelay = std::chrono::seconds(10));

  bool due() const;
};