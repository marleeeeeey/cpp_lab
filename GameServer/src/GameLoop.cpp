#include "GameLoop.h"

#include "GameSerialization/GameSerialization.h"
#include "GameShared/Simulation.h"
#include "GameShared/WorldSnapshot.h"
#include "ServerState.h"

// ----------------------------
// TODO: Options for GameLoop
// ----------------------------

// TODO: test game with 20 FPS on server and 60 FPS on client. It looks strange.
constexpr int SERVER_TICK_RATE_FPS = 60;  // should be the same as CLIENT_TICK_RATE_FPS
constexpr int WORLD_WIDTH = 800;
constexpr int WORLD_HEIGHT = 600;

GameLoop::GameLoop(std::shared_ptr<ServerState> state, BroadcastCb broadcastCb) {
  state_ = state;
  broadcastCb_ = broadcastCb;
}

void GameLoop::start() {
  gameThread_ = std::thread([this]() {
    // TODO: think about using MS everywhere in the Game instaed of float seconds
    int dtMs = 1000 / SERVER_TICK_RATE_FPS;
    float dtSeconds = 1.0f / SERVER_TICK_RATE_FPS;
    const auto tickDuration = std::chrono::milliseconds(dtMs);

    SPDLOG_INFO("GameLoop started");

    while (stopRequested_ == false) {
      auto start = std::chrono::steady_clock::now();

      {
        std::lock_guard lock(state_->gameSession->mutex);
        updateState_(state_, dtSeconds);
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

void GameLoop::updateState_(std::shared_ptr<ServerState> state, float dtSeconds) {
  for (PerSocketData* psd : state->gameSession->perSocketDatas) {
    auto& player = psd->player;
    simulatePlayer(player.state, dtSeconds, player.lastInput, WORLD_WIDTH, WORLD_HEIGHT);
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