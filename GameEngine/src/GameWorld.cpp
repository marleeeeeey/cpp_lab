#include "GameWorld.h"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "GameRenderer/IGameWorldRenderer.h"

static constexpr int MIN_PIXELS_PER_SECOND = 30;  // move at least this many pixels per second
static constexpr int MAX_PIXELS_PER_SECOND = 60;  // move this many pixels per second at most

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

GameWorld::GameWorld(std::weak_ptr<IGameWorldRenderer> gameWorldRenderer) {
  gameWorldRenderer_ = gameWorldRenderer;
}

void GameWorld::iterate(double elapsed, const GameInputData& userInputData) {
  impactOnSnowflakes_(elapsed, userInputData);
  impactOnPlayer_(elapsed, userInputData);
}

void GameWorld::onWindowSizeChanged(int width, int height) {
  windowWidth_ = width;
  windowHeight_ = height;
  updateSnowflakesCount_(width, height);
}

void GameWorld::setPlayerRandomPosition() const {
  auto renderer = gameWorldRenderer_.lock();
  if (!renderer) return;

  const float margin = 0.15f;
  const float minX = windowWidth_ * margin;
  const float maxX = windowWidth_ * (1.0f - margin);
  const float minY = windowHeight_ * margin;
  const float maxY = windowHeight_ * (1.0f - margin);
  renderer->myPlayer.position = glm::vec2(
      minX + SDL_randf() * (maxX - minX),
      minY + SDL_randf() * (maxY - minY));

  onPlayerPositionChanged();
}

void GameWorld::updateSnowflakesCount_(int width, int height) {
  auto renderer = gameWorldRenderer_.lock();
  if (!renderer) return;

  // ----------------------------------
  // Calculate numbers of snowflakes
  // ----------------------------------

  int numObjects = 0;
  const float area = width * height;
  constexpr float MEGAPIXEL = 1'000'000.0f;
  constexpr float OBJECTS_PER_MEGAPIXEL = 1300.0f;
  numObjects = static_cast<int>(std::lround(area * (OBJECTS_PER_MEGAPIXEL / MEGAPIXEL)));
  numObjects = std::clamp(numObjects, 0, 50'000);

  renderer->snowflakes.resize(numObjects);
  snowflakesSpeed_.resize(numObjects);

  // -------------------------------------------
  // Calculate snowflake positions and speeds
  // -------------------------------------------

  for (int i = 0; i < numObjects; i++) {
    glm::vec2& point = renderer->snowflakes[i];
    auto& pointSpeed = snowflakesSpeed_[i];

    if (point == glm::vec2{}) {
      point = glm::vec2(SDL_randf() * windowWidth_, SDL_randf() * windowHeight_);
    }

    if (pointSpeed == float{}) {
      pointSpeed = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
    }
  }
}

void GameWorld::impactOnSnowflakes_(double elapsed, const GameInputData& userInputData) {
  auto renderer = gameWorldRenderer_.lock();
  if (!renderer) return;

  glm::vec2 snowflakesDirection = glm::normalize(glm::vec2(1.0f, 1.0f));
  float acceleration = 1.0f;

  /* let's move all our gameDataForRendering.points a little for a new frame. */
  for (int i = 0; i < renderer->snowflakes.size(); i++) {
    glm::vec2& point = renderer->snowflakes[i];
    point += snowflakesSpeed_[i] * (float)elapsed * snowflakesDirection;

    // Generate new points if they go off the screen.
    if ((point.x >= windowWidth_) || (point.y >= windowHeight_)) {
      if (SDL_rand(2)) {  // Generate a new point on top of the screen
        point = glm::vec2(SDL_randf() * ((float)windowWidth_), 0.0f);
      } else {  // Generate a new point on the left of the screen
        point = glm::vec2(0.0f, SDL_randf() * ((float)windowHeight_));
      }

      snowflakesSpeed_[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND) * acceleration);
    }
  }
}

void GameWorld::impactOnPlayer_(double elapsed, const GameInputData& userInputData) {
  auto renderer = gameWorldRenderer_.lock();
  if (!renderer) {
    SPDLOG_ERROR("GameWorldRenderer is not initialized");
    return;
  }

  const float dt = static_cast<float>(elapsed);
  if (dt <= 0.0f) return;

  // ----------------------------------
  // Motion Params (px/s and px/s^2)
  // ----------------------------------

  const float maxSpeed = 260.0f;
  const float accel = 900.0f;
  const float decel = 1100.0f;

  // --------------
  // Read Input
  // --------------

  glm::vec2 dir(0.0f, 0.0f);
  if (userInputData.keyboard.held.up) dir.y -= 1.0f;
  if (userInputData.keyboard.held.down) dir.y += 1.0f;
  if (userInputData.keyboard.held.left) dir.x -= 1.0f;
  if (userInputData.keyboard.held.right) dir.x += 1.0f;

  // -----------------------
  // Normalize Speed
  // -----------------------

  const bool hasInput = (dir.x != 0.0f || dir.y != 0.0f);
  if (hasInput) {
    const float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    dir /= len;
  }

  const glm::vec2 targetVelocity = hasInput ? (dir * maxSpeed) : glm::vec2(0.0f);

  // ---------------------------
  // Accelerate/Decelerate
  // ---------------------------

  const float maxDelta = (hasInput ? accel : decel) * dt;
  playerVelocity_ = approachVec2(playerVelocity_, targetVelocity, maxDelta);

  // -----------------------
  // Minimal speed to stop
  // -----------------------

  const float stopEpsilon = 1.0f;
  if (!hasInput) {
    if (std::abs(playerVelocity_.x) < stopEpsilon) playerVelocity_.x = 0.0f;
    if (std::abs(playerVelocity_.y) < stopEpsilon) playerVelocity_.y = 0.0f;
  }

  // --------------------
  // Save old position
  // --------------------

  const glm::vec2 oldPos = renderer->myPlayer.position;

  // ---------------------------------
  // Move Player (px/s * s = px)
  // ---------------------------------

  renderer->myPlayer.position += playerVelocity_ * dt;

  // --------------------------
  // Wrap Player Position
  // --------------------------

  renderer->myPlayer.position.x = wrapCoord(renderer->myPlayer.position.x, static_cast<float>(windowWidth_));
  renderer->myPlayer.position.y = wrapCoord(renderer->myPlayer.position.y, static_cast<float>(windowHeight_));

  // -----------------------------
  // Notify if position changed
  // -----------------------------

  if (renderer->myPlayer.position != oldPos) {
    onPlayerPositionChanged();
  }
}
