#include "GameWorld.h"

#include <SDL3/SDL.h>

#include <glm/gtc/matrix_transform.hpp>

static constexpr int MIN_PIXELS_PER_SECOND = 30; /* move at least this many pixels per second. */
static constexpr int MAX_PIXELS_PER_SECOND = 60; /* move this many pixels per second at most. */

void GameWorld::updateObjectsCount_(int width, int height) {
  int numObjects = 0;

  if (auto sqrt = std::sqrt(width * height); sqrt != 0) {
    numObjects = width * height / sqrt;
  }

  gameDataForRendering_.points.resize(numObjects);
  pointsSpeed_.resize(numObjects);

  for (int i = 0; i < numObjects; i++) {
    glm::vec2& point = gameDataForRendering_.points[i];
    auto& pointSpeed = pointsSpeed_[i];

    if (point == glm::vec2{}) {
      point = glm::vec2(SDL_randf() * windowWidth_, SDL_randf() * windowHeight_);
    }

    if (pointSpeed == float{}) {
      pointSpeed = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
    }
  }
}

void GameWorld::init(int width, int height) {
  windowWidth_ = width;
  windowHeight_ = height;
  updateObjectsCount_(width, height);
}

void GameWorld::iterate(double elapsed, const UserInputData& userInputData) {
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

  /* let's move all our gameDataForRendering.points a little for a new frame. */
  for (int i = 0; i < gameDataForRendering_.points.size(); i++) {
    glm::vec2& point = gameDataForRendering_.points[i];
    point += pointsSpeed_[i] * (float)elapsed * globalDirection_;

    // Generate new points if they go off the screen.
    if ((point.x >= windowWidth_) || (point.y >= windowHeight_)) {
      if (SDL_rand(2)) {  // Generate a new point on top of the screen
        point = glm::vec2(SDL_randf() * ((float)windowWidth_), 0.0f);
      } else {  // Generate a new point on the left of the screen
        point = glm::vec2(0.0f, SDL_randf() * ((float)windowHeight_));
      }

      pointsSpeed_[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND) * acceleration_);
    }
  }
}

const GameDataForRendering& GameWorld::getGameDataForRendering() const {
  return gameDataForRendering_;
}

void GameWorld::onWindowSizeChanged(int width, int height) {
  windowWidth_ = width;
  windowHeight_ = height;
  updateObjectsCount_(width, height);
}
