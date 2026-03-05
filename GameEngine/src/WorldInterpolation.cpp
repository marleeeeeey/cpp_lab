#include "WorldInterpolation.h"

#include <spdlog/spdlog.h>

#include <algorithm>

#include "GameRenderer/IDebugRender.h"
#include "GameShared/SimulationTickRate.h"
#include "GameUtils/MakeScopeGuard.h"

// ------------------------------------
// TODO: Options for Interpolation
// ------------------------------------

// Interpolation delay in seconds.
// User will see not the latest world state from the server but state from the past:
// Latest world state - INTERPOLATION_DELAY_SECONDS (based on server tick rate 16ms).
// This delay is used to smooth out the latency between the client and server.
constexpr float INTERPOLATION_DELAY_SECONDS = 0.1f;

// Number of frames to store.
// It impacts on the maximum delay time (INTERPOLATION_DELAY_SECONDS) possible for interpolation.
// 32 frames equal to 32x16 = 512ms delay (16ms match to 60 Hz server tick rate).
constexpr size_t SIZE_OF_SNAPSHOTS_BUFFER = 32;

// ----------------------------
// Implementation
// ----------------------------

WorldInterpolation::WorldInterpolation(const ApplySnapshotToRenderCb& cb, std::weak_ptr<IDebugRender> debugRender) {
  applySnapshotToRender = cb;
  debugRender_ = debugRender;
}

void WorldInterpolation::addSnapshot(const WorldSnapshot& snapshot) {
  lastReceivedServerTick_ = snapshot.serverTick;
  estimatedServerTime_ = float(snapshot.serverTick) / CLIENT_INTERPOLATION_TICK_RATE;
  snapshotBuffer_.push_back(snapshot);

  // 32 is the max number of snapshots
  while (snapshotBuffer_.size() > 32)
    snapshotBuffer_.pop_front();
}

/*
 * Real Server Time  ----------------------------------------------->
 *
 *                      Latest received snapshot
 *                                 |
 *                                 v
 *    ----S100----S101----S102----S103----S104----S105
 *                                    ^
 *                                    |
 *                           estimatedServerTime
 *
 * Client renders NOT latest snapshot,
 * but some time in the past:
 *
 * renderTime = estimatedServerTime - INTERPOLATION_DELAY
 *
 *    ----S100----S101----S102----S103----S104----S105
 *                            ^
 *                            |
 *                        renderTick
 *
 * We find two snapshots around renderTick:
 *    ----S100----S101----S102----S103----S104----S105
 *                         |         |
 *                         A         B
 *                           ^
 *                           |
 *                        renderTick
 *
 * Then interpolate between A and B.
 */
void WorldInterpolation::iterate(float dt, float gameTime) {
  if (snapshotBuffer_.empty()) return;

  // --------------------------------------
  // Auto Apply Default Snapshot On Exit
  // --------------------------------------

  // Use snapshotForInterpolation pointer later to set the actual snapshot to render
  const WorldSnapshot* snapshotForInterpolation = &snapshotBuffer_.back();
  auto interpolateGuard = makeScopeGuard([this, &snapshotForInterpolation]() {
    if (applySnapshotToRender) {
      applySnapshotToRender(*snapshotForInterpolation);
    }
  });

  if (!isEnabled_) {
    return;
  }

  // --------------------------
  // Safe method to debug
  // --------------------------

  auto debug = [debugRender = debugRender_.lock()](const std::string& key, const std::string& line) {
    if (!debugRender) return;
    debugRender->addStaticLine(key, line);
  };

  // --------------------------------
  // Update time and tick counters
  // --------------------------------

  estimatedServerTime_ += dt;
  debug("interpolationDelaySeconds", std::format("Interpolation Delay: {} ms", INTERPOLATION_DELAY_SECONDS * 1000.0f));
  float renderTimeSeconds = std::max(0.0f, estimatedServerTime_ - INTERPOLATION_DELAY_SECONDS);
  uint32_t renderTick = int32_t(renderTimeSeconds * CLIENT_INTERPOLATION_TICK_RATE);  // Convert render time to tick space
  debug("renderTickDiff", std::format("Render Tick Diff: {}", lastReceivedServerTick_ - renderTick));

  // -----------------------------
  // Bad Frames Statistics
  // -----------------------------

  static uint32_t firstTickForThisClient = lastReceivedServerTick_;
  static uint32_t badFramesCounter = 0;
  badFramesCounter++;  // increment counter by default (correct for any return). Later decrement if we find a good frame
  uint32_t tickSinceFirstTick = lastReceivedServerTick_ - firstTickForThisClient;
  float percentOfBadFrames = badFramesCounter / (float)tickSinceFirstTick;
  debug("badFramesCounter", std::format("BAD frames counter: {}", badFramesCounter));
  debug("percentOfBadFrames", std::format("BAD frames: {:.2f} %", percentOfBadFrames * 100));

  // -----------------------
  // Not enough snapshots
  // -----------------------

  if (snapshotBuffer_.size() < 2) {
    if (!snapshotBuffer_.empty()) {
      SPDLOG_TRACE("Not enough snapshots for interpolation, using last snapshot");
      return;
    }
    SPDLOG_TRACE("Not enough snapshots for interpolation");
    return;
  }

  // --------------------------------------------
  // Find two snapshots around renderTick
  // --------------------------------------------

  const WorldSnapshot* A = nullptr;
  const WorldSnapshot* B = nullptr;

  for (size_t i = 0; i < snapshotBuffer_.size() - 1; ++i) {
    const auto& current = snapshotBuffer_[i];
    const auto& next = snapshotBuffer_[i + 1];

    if (current.serverTick <= renderTick &&
        next.serverTick >= renderTick) {
      A = &current;
      B = &next;

      debug("Buffer IDs", std::format("Found Buffer IDs: {} - {} from {}", i, i + 1, snapshotBuffer_.size()));
      break;
    }
  }

  if (!A || !B) {
    return;
  }

  // ----------------------------------------------
  // Frame Found: Update Good Stats
  // ----------------------------------------------

  badFramesCounter--;

  // -------------------------------
  // Compute interpolation factor
  // -------------------------------

  float tickDelta = float(B->serverTick - A->serverTick);
  debug("tickDelta", std::format("Tick Delta: {}", tickDelta));

  float t = 0.0f;
  if (tickDelta > 0.0f) {
    t = float(renderTick - A->serverTick) / tickDelta;
  }
  t = std::clamp(t, 0.0f, 1.0f);
  debug("Interpolation factor", std::format("Interpolation factor: {}", t));

  if (t == 0.0f) {
    // No interpolation, just use the first snapshot
    snapshotForInterpolation = A;
    return;
  }

  if (t == 1.0f) {
    // No interpolation, just use the second snapshot
    snapshotForInterpolation = B;
    return;
  }

  // -------------------------------
  // Build interpolated snapshot
  // -------------------------------

  WorldSnapshot interpolatedSnapshot;
  interpolatedSnapshot.serverTick = renderTick;
  interpolatedSnapshot.players.reserve(A->players.size());

  for (const auto& playerA : A->players) {
    // Find matching player in snapshot B
    auto it = std::ranges::find_if(
        B->players,
        [&](const PlayerSnapshot& p) {
          return p.id == playerA.id;
        });

    if (it == B->players.end()) {
      SPDLOG_WARN("Player {} not found in snapshot B", playerA.id);
      continue;
    }

    const auto& playerB = *it;

    // Linear interpolation of position
    glm::vec2 interpolatedPos = playerA.position + (playerB.position - playerA.position) * t;

    interpolatedSnapshot.players.push_back(
        PlayerSnapshot{
            playerA.id,
            interpolatedPos});
  }

  snapshotForInterpolation = &interpolatedSnapshot;
}
