#pragma once
#include <GameShared/InputPacket.h>

#include <cstdint>
#include <deque>
#include <optional>

#include "GameRenderer/IDebugRender.h"
#include "IWorldInterpolation.h"

class ClientWorldSimulation {
 public:
  // --------------
  // Constructor
  // --------------

  ClientWorldSimulation(std::weak_ptr<IDebugRender> debugRender);

  // -----------
  // Interface
  // -----------

  void addSnapshot(const WorldSnapshot& snapshot);
  void iterate(float dt, float gameTime);
  void setMyPlayerId(uint32_t playerId);
  void setMyLastInputPacket(const InputPacket& input);
  const WorldSnapshot& getResultSnapshot() const;

  // ----------------
  // Private state
  // ----------------

 private:
  std::deque<WorldSnapshot> snapshotBuffer_;  // Buffer of snapshots received from the server
  WorldSnapshot resultSnapshot_{};            // Will be used by Renderer. Probably renderer initiates interpolation
  std::weak_ptr<IDebugRender> debugRender_;
  std::optional<Player> myPlayer_;
  std::optional<PlayerId> myPlayerId_;
  float lastGameTimeSimulationCompleted_{};
};
