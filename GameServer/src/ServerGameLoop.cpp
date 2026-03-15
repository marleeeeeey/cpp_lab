#include "ServerGameLoop.h"

#include "GameSerialization/GameSerialization.h"
#include "GameShared/Simulation.h"
#include "GameShared/SimulationTickRate.h"
#include "GameShared/WorldSize.h"
#include "GameShared/WorldSnapshot.h"
#include "Profiler/Profiler.h"
#include "ServerState.h"

ServerGameLoop::ServerGameLoop(std::shared_ptr<ServerState> state, BroadcastCb broadcastCb) {
  state_ = state;
  broadcastCb_ = broadcastCb;
}

void ServerGameLoop::start() {
  gameThread_ = std::thread([this]() {
    SPDLOG_INFO("GameLoop started");

    // TODO: think about using MS everywhere in the Game instead of float seconds
    constexpr auto tickDuration = std::chrono::nanoseconds(1'000'000'000 / SERVER_SIMULATION_TICK_RATE);
    constexpr float dtSeconds = 1.0f / SERVER_SIMULATION_TICK_RATE;

    SPDLOG_INFO("Tick duration: {} ns",
                std::chrono::duration_cast<std::chrono::nanoseconds>(tickDuration).count());

    auto nextTick = std::chrono::steady_clock::now();

    while (stopRequested_ == false) {
      PROFILER_ZONE_NAMED("Server: Tick");
      nextTick += tickDuration;

      {
        PROFILER_ZONE_NAMED("Server: Simulation");
        std::lock_guard lock(state_->gameSession->mutex);
        updateState_(state_, dtSeconds);
        createWorldSnapshotAndSendToClients_(state_);
        state_->gameSession->tick++;
      }

      {
        PROFILER_ZONE_NAMED("Server: Sleep");
        auto now = std::chrono::steady_clock::now();
        if (nextTick < now) {
          nextTick = now;
        }
        std::this_thread::sleep_until(nextTick);
      }

      PROFILER_FRAME_MARK;
    }
  });
}

void ServerGameLoop::stop() {
  stopRequested_ = true;
  if (gameThread_.joinable()) {
    gameThread_.join();
  }
}

void ServerGameLoop::updateState_(std::shared_ptr<ServerState> state, float dtSeconds) {
  for (PerSocketData* psd : state->gameSession->perSocketDatas) {
    auto& player = psd->player;
    simulatePlayer(player.state, dtSeconds, player.lastInput.move, WORLD_WIDTH, WORLD_HEIGHT);
  }
}

void ServerGameLoop::createWorldSnapshotAndSendToClients_(std::shared_ptr<ServerState> state) const {
  WorldSnapshot world;
  world.serverTick = state->gameSession->tick;

  for (PerSocketData* psd : state->gameSession->perSocketDatas) {
    auto& player = psd->player;

    PlayerSnapshot snap;
    snap.id = player.id;
    snap.position = player.state.position;

    world.players.push_back(snap);
  }

  // Send the same snapshot to all clients
  auto payload = GameSerialization::serializeWorldSnapshot(world);
  broadcastCb_(GMT_WorldSnapshotFromServer, payload);
}