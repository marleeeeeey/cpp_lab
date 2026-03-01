#include "SnowflakesSimulation.h"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

#include "GameRenderer/IGameWorldRenderer.h"

static constexpr int MIN_PIXELS_PER_SECOND = 30;  // move at least this many pixels per second
static constexpr int MAX_PIXELS_PER_SECOND = 60;  // move this many pixels per second at most

namespace {
}  // namespace

SnowflakesSimulation::SnowflakesSimulation(std::weak_ptr<IGameWorldRenderer> gameWorldRenderer) {
  gameWorldRenderer_ = gameWorldRenderer;
}

void SnowflakesSimulation::iterate(double elapsed) {
  impactOnSnowflakes_(elapsed);
}

void SnowflakesSimulation::onWindowSizeChanged(int width, int height) {
  windowWidth_ = width;
  windowHeight_ = height;
  updateSnowflakesCount_(width, height);
}

void SnowflakesSimulation::updateSnowflakesCount_(int width, int height) {
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

void SnowflakesSimulation::impactOnSnowflakes_(double elapsed) {
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
