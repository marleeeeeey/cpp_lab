#include "GameShared/Simulation.h"

#include <spdlog/spdlog.h>

namespace {

float approach(float current, float target, float maxDelta) {
  const float delta = target - current;
  if (delta > maxDelta) return current + maxDelta;
  if (delta < -maxDelta) return current - maxDelta;
  return target;
}

glm::vec2 approachVec2(const glm::vec2& current, const glm::vec2& target, float maxDelta) {
  return glm::vec2(
      approach(current.x, target.x, maxDelta),
      approach(current.y, target.y, maxDelta));
}

float wrapCoord(float v, float maxExclusive) {
  if (maxExclusive <= 0.0f) return 0.0f;
  v = std::fmod(v, maxExclusive);
  if (v < 0.0f) v += maxExclusive;
  return v;
}

}  // namespace

void simulatePlayer(PlayerState& state, float dt, const glm::vec2& input,
                    float worldWidth, float worldHeight) {
  SPDLOG_TRACE("simulatePlayer: dt={}, input=({},{})", dt, input.x, input.y);

  // ----------------------------------
  // Motion Params (px/s and px/s^2)
  // ----------------------------------

  const float maxSpeed = 260.0f;
  const float accel = 900.0f;
  const float decel = 1100.0f;
  const float stopEpsilon = 1.0f;

  // --------------
  // Parse Input
  // --------------

  const bool hasInput = glm::length(input) > 0.0f;
  glm::vec2 dir = hasInput ? glm::normalize(input) : glm::vec2(0.0f);
  const glm::vec2 targetVelocity = dir * maxSpeed;

  // ---------------------------
  // Accelerate/Decelerate
  // ---------------------------

  const float maxDelta = (hasInput ? accel : decel) * dt;
  state.velocity = approachVec2(state.velocity, targetVelocity, maxDelta);

  // -----------------------
  // Minimal speed to stop
  // -----------------------

  if (!hasInput) {
    if (std::abs(state.velocity.x) < stopEpsilon) state.velocity.x = 0.0f;
    if (std::abs(state.velocity.y) < stopEpsilon) state.velocity.y = 0.0f;
  }
  // --------------------
  // Save old position
  // --------------------

  const glm::vec2 oldPos = state.position;

  // ---------------------------------
  // Move Player (px/s * s = px)
  // ---------------------------------

  state.position += state.velocity * dt;

  // --------------------------
  // Wrap Player Position
  // --------------------------

  state.position.x = wrapCoord(state.position.x, worldWidth);
  state.position.y = wrapCoord(state.position.y, worldHeight);

  // ------------------------------
  // Debug on position changed
  // ------------------------------

  if (oldPos != state.position) {
    SPDLOG_INFO("Player moved from ({}, {}) to ({}, {})", oldPos.x, oldPos.y, state.position.x, state.position.y);
  }
}
