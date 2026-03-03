#include "GameLoop.h"

#include "GameSerialization/GameSerialization.h"
#include "GameShared/Simulation.h"
#include "GameShared/WorldSnapshot.h"
#include "ServerState.h"

// ----------------------------
// TODO: Options for GameLoop
// ----------------------------

constexpr int TICK_RATE = 60;
constexpr int WORLD_WIDTH = 800;
constexpr int WORLD_HEIGHT = 600;

GameLoop::GameLoop(std::shared_ptr<ServerState> state, BroadcastCb broadcastCb) {
  state_ = state;
  broadcastCb_ = broadcastCb;
}

void GameLoop::start() {
  gameThread_ = std::thread([this]() {
    const auto tickDuration = std::chrono::milliseconds(1000 / TICK_RATE);

    SPDLOG_INFO("GameLoop started");

    while (stopRequested_ == false) {
      auto start = std::chrono::steady_clock::now();

      {
        std::lock_guard lock(state_->gameSession->mutex);
        updateState_(state_);
        sendStateToClients_(state_);
        state_->gameSession->tick++;
      }

      std::this_thread::sleep_until(start + tickDuration);
    }
  });
}

void GameLoop::stop() {
  stopRequested_ = true;
  if (gameThread_.joinable()) {
    gameThread_.join();
  }
}

void GameLoop::updateState_(std::shared_ptr<ServerState> state) {
  for (PerSocketData* psd : state->gameSession->perSocketDatas) {
    auto& player = psd->player;
    SPDLOG_TRACE("Player {}: pos=({},{})", player.id, player.state.position.x, player.state.position.y);
    SPDLOG_TRACE("Player {}: input=({},{})", player.id, player.lastInput.x, player.lastInput.y);
    float dt = 1.0f / static_cast<float>(TICK_RATE);
    simulatePlayer(player.state, dt, player.lastInput, WORLD_WIDTH, WORLD_HEIGHT);
  }
}

void GameLoop::sendStateToClients_(std::shared_ptr<ServerState> state) {
  WorldSnapshot world;
  world.serverTick = state->gameSession->tick;

  for (PerSocketData* psd : state->gameSession->perSocketDatas) {
    auto& player = psd->player;

    PlayerSnapshot snap;
    snap.id = player.id;
    snap.position = player.state.position;

    world.players.push_back(snap);
  }

  if (!world.players.empty()) {
    auto& p1 = world.players[0];
    SPDLOG_TRACE("Sending WorldSnapshot: p1 pos=({},{}) ptr={}", p1.position.x, p1.position.y, (uint64_t)&state->gameSession->perSocketDatas[0]->player);
  }

  // Send the same snapshot to all clients
  auto payload = GameSerialization::serializeWorldSnapshot(world);
  broadcastCb_(GMT_WorldSnapshot, payload);
}