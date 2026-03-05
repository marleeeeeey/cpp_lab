#include "ClientWorldSimulation.h"

#include <GameShared/Simulation.h>
#include <GameShared/WorldSize.h>
#include <spdlog/spdlog.h>

#include "GameRenderer/IDebugRender.h"
#include "GameShared/Simulation.h"
#include "GameShared/SimulationTickRate.h"
#include "GameUtils/MakeScopeGuard.h"

// ------------------------------------
// TODO: Options for Interpolation
// ------------------------------------

// Number of frames to store.
// It impacts on the maximum delay time (INTERPOLATION_DELAY_SECONDS) possible for interpolation.
// 32 frames equal to 32x16 = 512ms delay (16ms match to 60 Hz server tick rate).
constexpr size_t SIZE_OF_SNAPSHOTS_BUFFER = 32;

// ----------------------------
// Implementation
// ----------------------------

ClientWorldSimulation::ClientWorldSimulation(std::weak_ptr<IDebugRender> debugRender) {
  debugRender_ = debugRender;
}

void ClientWorldSimulation::addSnapshot(const WorldSnapshot& snapshot) {
  snapshotBuffer_.push_back(snapshot);
  // 32 is the max number of snapshots
  while (snapshotBuffer_.size() > 32)
    snapshotBuffer_.pop_front();
}

void ClientWorldSimulation::iterate(float dt, float gameTime) {
  auto debug = [debugRender = debugRender_.lock()](const std::string& key, const std::string& line) {
    if (!debugRender) return;
    debugRender->addStaticLine(key, line);
  };

  debug("dt", std::format("dt: {}", dt));
  debug("gameTime", std::format("gameTime: {}", gameTime));

  if (snapshotBuffer_.empty()) return;

  const auto& latestSnapshotFromServer = snapshotBuffer_.back();
  resultSnapshot_ = latestSnapshotFromServer;

  for (auto& playerSnapshot : resultSnapshot_.players) {
    if (myPlayerId_ && playerSnapshot.id == myPlayerId_.value()) {
      // For the first time init a local player from the server package
      if (!myPlayer_) {
        myPlayer_ = Player{
            .id = playerSnapshot.id,
            .name = {},
            .messagesSent = {},
            .state = {
                .position = playerSnapshot.position,
                .velocity = {},
            },
            .lastInput = {}};
        SPDLOG_INFO("My player id: {}", myPlayer_->id);
      }

      // For local player simulation should be done locally
      simulatePlayer(myPlayer_->state, dt, myPlayer_->lastInput, WORLD_WIDTH, WORLD_HEIGHT);

      // Modify local player after simulation
      // TODO: think how to fix server-client discrepancy simulation
      playerSnapshot.position = myPlayer_->state.position;
    }
  }
}

void ClientWorldSimulation::setMyPlayerId(uint32_t playerId) {
  myPlayerId_ = playerId;
}

void ClientWorldSimulation::setMyLastInputPacket(const InputPacket& input) {
  if (!myPlayer_) return;
  myPlayer_->lastInput.x = input.moveX;
  myPlayer_->lastInput.y = input.moveY;
}

const WorldSnapshot& ClientWorldSimulation::getResultSnapshot() const {
  return resultSnapshot_;
}
