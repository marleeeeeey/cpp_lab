#include "GameWorld.h"

#include "GlobalConstants.h"

static constexpr int MIN_PIXELS_PER_SECOND = 30; /* move at least this many pixels per second. */
static constexpr int MAX_PIXELS_PER_SECOND = 60; /* move this many pixels per second at most. */

void GameWorld::init() {
  static constexpr int NUM_POINTS = 500;
  gameDataForRendering.points.resize(NUM_POINTS);
  point_speeds.resize(NUM_POINTS);

  /* set up the data for a bunch of gameDataForRendering.points. */
  for (int i = 0; i < NUM_POINTS; i++) {
    gameDataForRendering.points[i].x = SDL_randf() * ((float)WINDOW_WIDTH);
    gameDataForRendering.points[i].y = SDL_randf() * ((float)WINDOW_HEIGHT);
    point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
  }
}

void GameWorld::iterate(double elapsed, const UserInputData& userInputData) {
  if (userInputData.pressed.up) {
    acceleration += 0.05f;
    SDL_Log("acceleration: %f", acceleration);
  } else if (userInputData.pressed.down) {
    acceleration -= 0.05f;
    SDL_Log("acceleration: %f", acceleration);
  }

  /* let's move all our gameDataForRendering.points a little for a new frame. */
  for (int i = 0; i < gameDataForRendering.points.size(); i++) {
    const float distance = elapsed * point_speeds[i];
    gameDataForRendering.points[i].x += distance;
    gameDataForRendering.points[i].y += distance;
    if ((gameDataForRendering.points[i].x >= WINDOW_WIDTH) || (gameDataForRendering.points[i].y >= WINDOW_HEIGHT)) {
      /* off the screen; restart it elsewhere! */
      if (SDL_rand(2)) {
        gameDataForRendering.points[i].x = SDL_randf() * ((float)WINDOW_WIDTH);
        gameDataForRendering.points[i].y = 0.0f;
      } else {
        gameDataForRendering.points[i].x = 0.0f;
        gameDataForRendering.points[i].y = SDL_randf() * ((float)WINDOW_HEIGHT);
      }
      point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND) * acceleration);
    }
  }
}

const GameDataForRendering& GameWorld::getGameDataForRendering() const {
  return gameDataForRendering;
}