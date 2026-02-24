#pragma once

#include <cstdint>
#include <functional>
#include <vector>

class GameTimer final {
 public:
  // --------------
  // Signatures
  // --------------

  using Callback = std::function<void()>;
  using TimerId = std::uint64_t;

  GameTimer() = default;

  // --------------
  // Interface
  // --------------

  // elapsedSeconds — frame delta time (in seconds)
  void iterate(float elapsedSeconds);

  void pause();
  void resume();
  void reset();  // resets time and clears all scheduled tasks
  bool isPaused() const { return paused_; }

  // "game time" (stops when paused)
  float time() const { return timeSeconds_; }

  // Unity-like Invoke: run once after delaySeconds (in game time)
  TimerId scheduleOnce(float delaySeconds, Callback cb);

  // Unity-like InvokeRepeating:
  // firstDelaySeconds - delay before the first run
  // intervalSeconds - repeating interval
  // repeatCount: -1 = infinite, otherwise number of executions
  TimerId scheduleRepeating(float firstDelaySeconds,
                            float intervalSeconds,
                            int repeatCount,
                            Callback cb);

  // Cancel a timer by id (like CancelInvoke via a handle)
  bool cancel(TimerId id);

  // Cancel everything
  void cancelAll();

  // --------------
  // Details
  // --------------

 private:
  struct Task {
    TimerId id{};
    float nextFireTime{};  // absolute "game time" moment (seconds) when to fire
    float interval{};      // 0 => one-shot
    int remaining{};       // -1 infinite, otherwise how many executions are left
    Callback cb{};
    bool cancelled{false};
  };

  TimerId nextId_ = 1;
  bool paused_ = false;
  float timeSeconds_ = 0.0f;
  std::vector<Task> tasks_;

  TimerId allocateId_();
};