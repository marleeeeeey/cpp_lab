#pragma once
#include "GameShared/WorldSnapshot.h"

class IWorldInterpolation {
 public:
  // -------------
  // Signatures
  // -------------

  using ApplySnapshotToRenderCb = std::function<void(const WorldSnapshot&)>;

  // ----------
  // Factory
  // ----------

  virtual ~IWorldInterpolation() = default;

  // ------------
  // Interface
  // ------------

  // Called when snapshot accepted from network
  virtual void addSnapshot(const WorldSnapshot& snapshot) = 0;

  // Iterate interpolation and call a callback to apply snapshot to render
  // dt - time since the last iteration (in seconds)
  // gameTime - time since game begin (in seconds)
  virtual void iterate(float dt, float gameTime) = 0;

  virtual void setEnabled(bool enabled) = 0;
  virtual bool isEnabled() const = 0;

  virtual void setMyPlayerId(uint32_t playerId) = 0;
  virtual void setMyLastInputPacket(const InputPacket& input) = 0;
};
