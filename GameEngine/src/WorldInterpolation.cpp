#include "WorldInterpolation.h"

#include <spdlog/spdlog.h>

#include <algorithm>

#include "GameRenderer/IDebugRender.h"
#include "GameUtils/MakeScopeGuard.h"

WorldInterpolation::WorldInterpolation(const InterpolatedCb& cb, std::weak_ptr<IDebugRender> debugRender) {
  interpolatedCb_ = cb;
  debugRender_ = debugRender;
}

void WorldInterpolation::addSnapshot(const WorldSnapshot& snapshot) {
  lastReceivedServerTick_ = snapshot.serverTick;
  estimatedServerTime_ = float(snapshot.serverTick) / tickRate_;
  snapshotBuffer_.push_back(snapshot);

  // 32 is the max number of snapshots
  while (snapshotBuffer_.size() > 32)
    snapshotBuffer_.pop_front();
}

void WorldInterpolation::iterate(float elapsed) {
  auto scopeGuard = makeScopeGuard([&] {
    // Advance estimated server time smoothly at the end of the iteration.
    // Because we have just received a snapshot. This advance should be applied to the next frame.
    estimatedServerTime_ += elapsed;
  });

  // Safe method to debug
  auto debug = [debugRender = debugRender_.lock()](const std::string& key, const std::string& line) {
    if (!debugRender) return;
    debugRender->addStaticLine(key, line);
  };

  constexpr float interpolationDelaySeconds = 0.045f;  // 45 ms
  debug("interpolationDelaySeconds", std::format("Interpolation Delay: {} ms", interpolationDelaySeconds * 1000.0f));
  float renderTimeSeconds = std::max(0.0f, estimatedServerTime_ - interpolationDelaySeconds);

  // Convert render time to tick space
  uint32_t renderTick = uint32_t(renderTimeSeconds * tickRate_);

  debug("renderTickDiff", std::format("Render Tick Diff: -{}", lastReceivedServerTick_ - renderTick));

  // --------------------------------------------
  // Not enough snapshots
  // --------------------------------------------

  if (snapshotBuffer_.size() < 2) {
    if (!snapshotBuffer_.empty()) {
      SPDLOG_WARN("Not enough snapshots for interpolation, using last snapshot");
      return;
    }
    SPDLOG_ERROR("Not enough snapshots for interpolation");
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
      break;
    }
  }

  // Fallback if no valid pair was found
  if (!A || !B) {
    SPDLOG_WARN("No valid snapshot pair found, using last snapshot");
    return;
  }

  // --------------------------------------------
  // Compute interpolation factor
  // --------------------------------------------

  float tickDelta = float(B->serverTick - A->serverTick);

  debug("tickDelta", std::format("Tick Delta: {}", tickDelta));  // TODO: problem: Tick Delta always "1"

  float t = 0.0f;
  if (tickDelta > 0.0f) {
    t = float(renderTick - A->serverTick) / tickDelta;
  }

  t = std::clamp(t, 0.0f, 1.0f);

  debug("Interpolation factor", std::format("Interpolation factor: {}", t));

  if (t == 0.0f) {
    // No interpolation, just use the first snapshot
    interpolatedCb_(*A);
  }

  if (t == 1.0f) {
    // No interpolation, just use the second snapshot
    interpolatedCb_(*B);
  }

  // --------------------------------------------
  // Build interpolated snapshot
  // --------------------------------------------

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

  // -----------------------------
  // Notify about interpolation
  // -----------------------------

  if (!interpolatedCb_) {
    SPDLOG_WARN("No interpolated callback set");
    return;
  }

  interpolatedCb_(interpolatedSnapshot);
}
