#include "GameShared/Simulation.h"

#include <spdlog/spdlog.h>

#include "GameUtils/GeomUtils.h"

// -----------------------------------------
// Player Motion Params (px/s and px/s^2)
// -----------------------------------------

constexpr float PLAYER_MAX_SPEED = 260.0f;
constexpr float PLAYER_ACCEL = 900.0f;
constexpr float PLAYER_DECEL = 1100.0f;
constexpr float PLAYER_STOP_EPSILON = 1.0f;

void simulatePlayer(PlayerState& state, float dtSeconds, const glm::vec2& input,
                    float worldWidth, float worldHeight) {
  SPDLOG_TRACE("simulatePlayer: dt={}, input=({},{})", dtSeconds, input.x, input.y);

  // --------------
  // Parse Input
  // --------------

  const bool hasInput = glm::length(input) > 0.0f;
  glm::vec2 dir = hasInput ? glm::normalize(input) : glm::vec2(0.0f);
  const glm::vec2 targetVelocity = dir * PLAYER_MAX_SPEED;

  // ---------------------------
  // Accelerate/Decelerate
  // ---------------------------

  const float maxDelta = (hasInput ? PLAYER_ACCEL : PLAYER_DECEL) * dtSeconds;
  state.velocity = GeomUtils::approachVec2(state.velocity, targetVelocity, maxDelta);

  // -----------------------
  // Minimal speed to stop
  // -----------------------

  if (!hasInput) {
    if (std::abs(state.velocity.x) < PLAYER_STOP_EPSILON) state.velocity.x = 0.0f;
    if (std::abs(state.velocity.y) < PLAYER_STOP_EPSILON) state.velocity.y = 0.0f;
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

PlayerState interpolatePlayerPosition(const PlayerState& from, const PlayerState& to, float deltaTime) {
  PlayerState result;

  result.position = glm::mix(from.position, to.position, deltaTime);
  result.velocity = glm::mix(from.velocity, to.velocity, deltaTime);

  return result;
}