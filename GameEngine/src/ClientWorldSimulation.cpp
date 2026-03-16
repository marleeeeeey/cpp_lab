#include "ClientWorldSimulation.h"

#include <GameShared/Simulation.h>
#include <GameShared/WorldSize.h>
#include <spdlog/spdlog.h>

#include "GameRenderer/IDebugRender.h"

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
  dataForRenderer_ = std::make_shared<DataForRenderer>();
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

  debug("dt", std::format("dt: {:.3f} seconds", dt));
  debug("gameTime", std::format("gameTime: {:.1f} seconds", gameTime));

  if (snapshotBuffer_.empty()) return;

  const auto& latestSnapshotFromServer = snapshotBuffer_.back();
  dataForRenderer_->snapshot = latestSnapshotFromServer;

  for (auto& playerSnapshot : dataForRenderer_->snapshot.players) {
    if (myPlayerId_ && playerSnapshot.id == myPlayerId_.value()) {
      // For the first time init a local player from the server package
      auto& localPlayer = dataForRenderer_->localPlayer;
      if (!localPlayer) {
        localPlayer = Player{
            .id = playerSnapshot.id,
            .name = {},
            .messagesSent = {},
            .state = {
                .position = playerSnapshot.position,
                .velocity = {},
            },
            .lastInput = {}};
        SPDLOG_INFO("My player id: {}", dataForRenderer_->localPlayer->id);
      }

      // For local player simulation should be done locally
      simulatePlayer(localPlayer->state, dt, localPlayer->lastInput.move, WORLD_WIDTH, WORLD_HEIGHT);
    }
  }
}

void ClientWorldSimulation::setMyPlayerId(uint32_t playerId) {
  myPlayerId_ = playerId;
}

void ClientWorldSimulation::setMyLastInputPacket(const InputPacket& input) {
  if (!dataForRenderer_->localPlayer) return;
  dataForRenderer_->localPlayer->lastInput.move.x = input.moveX;
  dataForRenderer_->localPlayer->lastInput.move.y = input.moveY;
}

std::shared_ptr<DataForRenderer> ClientWorldSimulation::getDataForRenderer() const {
  return dataForRenderer_;
}
