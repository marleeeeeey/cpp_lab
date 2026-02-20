#include "GameWorld.h"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "GameRenderer/IGameWorldRenderer.h"

static constexpr int MIN_PIXELS_PER_SECOND = 30; /* move at least this many pixels per second. */
static constexpr int MAX_PIXELS_PER_SECOND = 60; /* move this many pixels per second at most. */

GameWorld::GameWorld(int width, int height, std::weak_ptr<IGameWorldRenderer> gameWorldRenderer) {
  windowWidth_ = width;
  windowHeight_ = height;
  gameWorldRenderer_ = gameWorldRenderer;
  updateSnowflakesCount_(width, height);
}

void GameWorld::iterate(double elapsed, const UserInputData& userInputData) {
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
  renderer->myPlayer.position = glm::vec2(SDL_randf() * windowWidth_, SDL_randf() * windowHeight_);
  onPlayerPositionChanged();
}

void GameWorld::updateSnowflakesCount_(int width, int height) {
  auto renderer = gameWorldRenderer_.lock();
  if (!renderer) return;

  int numObjects = 0;

  if (auto sqrt = std::sqrt(width * height); sqrt != 0) {
    numObjects = width * height / sqrt;
  }

  renderer->snowflakes.resize(numObjects);
  snowflakesSpeed_.resize(numObjects);

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

void GameWorld::impactOnSnowflakes_(double elapsed, const UserInputData& userInputData) {
  if constexpr (false) {
    // Pressed keys change acceleration or rotation direction.
    float accelerationShift = elapsed * 5.f;
    float rotateAngleDeg = elapsed * 100.0f;
    if (userInputData.held.up) {
      acceleration_ += accelerationShift;
    } else if (userInputData.held.down) {
      acceleration_ -= accelerationShift;
    } else if (userInputData.held.left) {
      glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotateAngleDeg), glm::vec3(0.0f, 0.0f, 1.0f));
      globalDirection_ = glm::vec2(rotation * glm::vec4(globalDirection_, 0.0f, 0.0f));
    } else if (userInputData.held.right) {
      glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-rotateAngleDeg), glm::vec3(0.0f, 0.0f, 1.0f));
      globalDirection_ = glm::vec2(rotation * glm::vec4(globalDirection_, 0.0f, 0.0f));
    }
  }

  auto renderer = gameWorldRenderer_.lock();
  if (!renderer) return;

  /* let's move all our gameDataForRendering.points a little for a new frame. */
  for (int i = 0; i < renderer->snowflakes.size(); i++) {
    glm::vec2& point = renderer->snowflakes[i];
    point += snowflakesSpeed_[i] * (float)elapsed * globalDirection_;

    // Generate new points if they go off the screen.
    if ((point.x >= windowWidth_) || (point.y >= windowHeight_)) {
      if (SDL_rand(2)) {  // Generate a new point on top of the screen
        point = glm::vec2(SDL_randf() * ((float)windowWidth_), 0.0f);
      } else {  // Generate a new point on the left of the screen
        point = glm::vec2(0.0f, SDL_randf() * ((float)windowHeight_));
      }

      snowflakesSpeed_[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND) * acceleration_);
    }
  }
}

void GameWorld::impactOnPlayer_(double elapsed, const UserInputData& userInputData) {
  auto renderer = gameWorldRenderer_.lock();
  if (!renderer) {
    SPDLOG_ERROR("GameWorldRenderer is not initialized");
  }
  float step = 10.0f;
  bool positionChanged = true;

  if (userInputData.pressed.up)
    renderer->myPlayer.position.y -= step;
  else if (userInputData.pressed.down)
    renderer->myPlayer.position.y += step;
  else if (userInputData.pressed.left)
    renderer->myPlayer.position.x -= step;
  else if (userInputData.pressed.right)
    renderer->myPlayer.position.x += step;
  else
    positionChanged = false;

  if (positionChanged) {
    onPlayerPositionChanged();
  }
}
