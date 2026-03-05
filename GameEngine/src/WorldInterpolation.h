#pragma once
#include <GameShared/InputPacket.h>

#include <cstdint>
#include <deque>

#include "GameRenderer/IDebugRender.h"
#include "IWorldInterpolation.h"

class WorldInterpolation : public IWorldInterpolation {
 public:
  // --------------
  // Constructor
  // --------------

  WorldInterpolation(const ApplySnapshotToRenderCb& cb, std::weak_ptr<IDebugRender> debugRender);

  // --------------------------------
  // Interface IWorldInterpolation
  // --------------------------------

  void addSnapshot(const WorldSnapshot& snapshot) override;
  void iterate(float dt, float gameTime) override;
  void setEnabled(bool enabled) override { isEnabled_ = enabled; }
  bool isEnabled() const override { return isEnabled_; }
  void setMyPlayerId(uint32_t playerId) override {}
  void setMyLastInputPacket(const InputPacket& input) override {};

  // ----------------
  // Private state
  // ----------------

 private:
  float estimatedServerTime_ = 0.0f;              // (seconds)
  uint32_t lastReceivedServerTick_ = 0;           // Latest tick received from server
  std::deque<WorldSnapshot> snapshotBuffer_;      // Buffer of snapshots received from server
  ApplySnapshotToRenderCb applySnapshotToRender;  // Callback to call when interpolated snapshot is ready
  std::weak_ptr<IDebugRender> debugRender_;
  bool isEnabled_{false};  // TODO: This is the bad interpolation disabled by default
};
