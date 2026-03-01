#include "GameUtils/GameTimer.h"

#include <algorithm>

GameTimer::TimerId GameTimer::allocateId_() {
  return nextId_++;
}

void GameTimer::pause() {
  paused_ = true;
}

void GameTimer::resume() {
  paused_ = false;
}

void GameTimer::reset() {
  paused_ = false;
  timeSeconds_ = 0.0f;
  tasks_.clear();
  nextId_ = 1;
}

void GameTimer::cancelAll() {
  tasks_.clear();
}

bool GameTimer::cancel(TimerId id) {
  bool found = false;
  for (auto& t : tasks_) {
    if (t.id == id) {
      t.cancelled = true;
      found = true;
    }
  }
  // Lazy removal — we'll clean it up in iterate()
  return found;
}

GameTimer::TimerId GameTimer::scheduleOnce(float delaySeconds, Callback cb) {
  if (!cb) return 0;

  delaySeconds = std::max(0.0f, delaySeconds);

  Task t;
  t.id = allocateId_();
  t.nextFireTime = timeSeconds_ + delaySeconds;
  t.interval = 0.0f;
  t.remaining = 1;
  t.cb = std::move(cb);

  tasks_.push_back(std::move(t));
  return tasks_.back().id;
}

GameTimer::TimerId GameTimer::scheduleRepeating(float firstDelaySeconds,
                                                float intervalSeconds,
                                                int repeatCount,
                                                Callback cb) {
  if (!cb) return 0;

  firstDelaySeconds = std::max(0.0f, firstDelaySeconds);
  intervalSeconds = std::max(0.0f, intervalSeconds);

  Task t;
  t.id = allocateId_();
  t.nextFireTime = timeSeconds_ + firstDelaySeconds;
  t.interval = intervalSeconds;
  t.remaining = repeatCount;  // -1 => infinite
  t.cb = std::move(cb);

  // If an interval == 0 and repeatCount != 1, it will still be "at most once per frame",
  // but for a true one-shot prefer scheduleOnce().
  tasks_.push_back(std::move(t));
  return tasks_.back().id;
}

void GameTimer::iterate(float elapsedSeconds) {
  if (paused_) return;
  if (elapsedSeconds <= 0.0f) return;

  timeSeconds_ += elapsedSeconds;

  // 1) Execute all ready tasks.
  // Note: callbacks may schedule new tasks — that's OK, we iterate by index.
  for (std::size_t i = 0; i < tasks_.size(); ++i) {
    auto& t = tasks_[i];
    if (t.cancelled) continue;

    if (timeSeconds_ + 1e-6f < t.nextFireTime) continue;

    // Execute
    if (t.cb) t.cb();

    if (t.cancelled) continue;

    // Decrement remaining counter
    if (t.remaining > 0) {
      --t.remaining;
    }

    // Decide: remove or reschedule
    const bool finished = (t.remaining == 0);
    if (finished) {
      t.cancelled = true;
      continue;
    }

    // Reschedule
    if (t.interval <= 0.0f) {
      // interval=0: to avoid looping inside the same frame, push to the next tick
      t.nextFireTime = timeSeconds_;
    } else {
      // Regular repeating interval
      t.nextFireTime += t.interval;

      // If elapsedSeconds was large, and we overshot, don't "catch up" with a burst.
      // Instead, jump forward to the next valid time slot (typical game-timer behavior).
      if (t.nextFireTime < timeSeconds_) {
        const float behind = timeSeconds_ - t.nextFireTime;
        const int steps = static_cast<int>(behind / t.interval) + 1;
        t.nextFireTime += steps * t.interval;
      }
    }
  }

  // 2) Cleanup cancelled/finished tasks
  tasks_.erase(std::remove_if(tasks_.begin(), tasks_.end(),
                              [](const Task& t) { return t.cancelled; }),
               tasks_.end());
}