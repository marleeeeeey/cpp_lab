#include "GameWorld.h"

#include <SDL3/SDL.h>

#include <glm/gtc/matrix_transform.hpp>

#include "GlobalConstants.h"

static constexpr int MIN_PIXELS_PER_SECOND = 30; /* move at least this many pixels per second. */
static constexpr int MAX_PIXELS_PER_SECOND = 60; /* move this many pixels per second at most. */

void GameWorld::init() {
  static constexpr int NUM_POINTS = 500;
  gameDataForRendering.points.resize(NUM_POINTS);
  point_speeds.resize(NUM_POINTS);

  /* set up the data for a bunch of gameDataForRendering.points. */
  for (int i = 0; i < NUM_POINTS; i++) {
    glm::vec2& point = gameDataForRendering.points[i];

    point = glm::vec2(
        SDL_randf() * ((float)WINDOW_WIDTH),
        SDL_randf() * ((float)WINDOW_HEIGHT));

    point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
  }
}

void GameWorld::iterate(double elapsed, const UserInputData& userInputData) {
  // Pressed keys change acceleration or rotation direction.
  float accelerationShift = elapsed * 5.f;
  float rotateAngleDeg = elapsed * 100.0f;
  if (userInputData.held.up) {
    acceleration += accelerationShift;
    SDL_Log("acceleration: %f", acceleration);
  } else if (userInputData.held.down) {
    acceleration -= accelerationShift;
    SDL_Log("acceleration: %f", acceleration);
  } else if (userInputData.held.left) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotateAngleDeg), glm::vec3(0.0f, 0.0f, 1.0f));
    global_direction = glm::vec2(rotation * glm::vec4(global_direction, 0.0f, 0.0f));
  } else if (userInputData.held.right) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-rotateAngleDeg), glm::vec3(0.0f, 0.0f, 1.0f));
    global_direction = glm::vec2(rotation * glm::vec4(global_direction, 0.0f, 0.0f));
  }

  /* let's move all our gameDataForRendering.points a little for a new frame. */
  for (int i = 0; i < gameDataForRendering.points.size(); i++) {
    glm::vec2& point = gameDataForRendering.points[i];
    point += point_speeds[i] * (float)elapsed * global_direction;

    // Generate new points if they go off the screen.
    if ((point.x >= WINDOW_WIDTH) || (point.y >= WINDOW_HEIGHT)) {
      if (SDL_rand(2)) {  // Generate a new point on top of the screen
        point = glm::vec2(SDL_randf() * ((float)WINDOW_WIDTH), 0.0f);
      } else {  // Generate a new point on the left of the screen
        point = glm::vec2(0.0f, SDL_randf() * ((float)WINDOW_HEIGHT));
      }

      point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND) * acceleration);
    }
  }
}

const GameDataForRendering& GameWorld::getGameDataForRendering() const {
  return gameDataForRendering;
}
