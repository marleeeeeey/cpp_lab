#include "GameShared/Simulation.h"

#include <spdlog/spdlog.h>

#include "GameUtils/GeomUtils.h"

void simulatePlayer(PlayerState& state, float dtSeconds, const glm::vec2& input,
                    float worldWidth, float worldHeight) {
  SPDLOG_TRACE("simulatePlayer: dt={}, input=({},{})", dtSeconds, input.x, input.y);

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

  const float maxDelta = (hasInput ? accel : decel) * dtSeconds;
  state.velocity = GeomUtils::approachVec2(state.velocity, targetVelocity, maxDelta);

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

  state.position += state.velocity * dtSeconds;

  // --------------------------
  // Wrap Player Position
  // --------------------------

  state.position.x = GeomUtils::wrapCoord(state.position.x, worldWidth);
  state.position.y = GeomUtils::wrapCoord(state.position.y, worldHeight);
}
