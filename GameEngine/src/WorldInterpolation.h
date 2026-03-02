#pragma once
#include <cstdint>
#include <deque>

#include "GameRenderer/IDebugRender.h"
#include "GameShared/WorldSnapshot.h"

class WorldInterpolation {
 public:
  // -------------
  // Signatures
  // -------------

  using InterpolatedCb = std::function<void(const WorldSnapshot&)>;

  // --------------
  // Constructor
  // --------------

  WorldInterpolation(const InterpolatedCb& cb, std::weak_ptr<IDebugRender> debugRender);

  // ------------
  // Interface
  // ------------

  // Called when snapshot accepted from network. Reset`estimatedServerTick`.
  void addSnapshot(const WorldSnapshot& snapshot);

  // Returns interpolated snapshot
  void iterate(float elapsed);

  // ----------------
  // Private state
  // ----------------

 private:
  float estimatedServerTime_ = 0.0f;          // (seconds)
  uint32_t lastReceivedServerTick_ = 0;       // Latest tick received from server
  static constexpr float tickRate_ = 60.0f;   // TODO: make it global (per second)
  std::deque<WorldSnapshot> snapshotBuffer_;  // Buffer of snapshots received from server
  InterpolatedCb interpolatedCb_;             // Callback to call when interpolated snapshot is ready

  std::weak_ptr<IDebugRender> debugRender_;
};
