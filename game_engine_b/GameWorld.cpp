#include "GameWorld.h"

#include "GlobalConstants.h"

static constexpr int MIN_PIXELS_PER_SECOND = 30; /* move at least this many pixels per second. */
static constexpr int MAX_PIXELS_PER_SECOND = 60; /* move this many pixels per second at most. */

void GameWorld::init() {
  static constexpr int NUM_POINTS = 500;
  points.resize(NUM_POINTS);
  point_speeds.resize(NUM_POINTS);

  /* set up the data for a bunch of points. */
  for (int i = 0; i < NUM_POINTS; i++) {
    points[i].x = SDL_randf() * ((float)WINDOW_WIDTH);
    points[i].y = SDL_randf() * ((float)WINDOW_HEIGHT);
    point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
  }
}

void GameWorld::iterate(double elapsed) {
  /* let's move all our points a little for a new frame. */
  for (int i = 0; i < points.size(); i++) {
    const float distance = elapsed * point_speeds[i];
    points[i].x += distance;
    points[i].y += distance;
    if ((points[i].x >= WINDOW_WIDTH) || (points[i].y >= WINDOW_HEIGHT)) {
      /* off the screen; restart it elsewhere! */
      if (SDL_rand(2)) {
        points[i].x = SDL_randf() * ((float)WINDOW_WIDTH);
        points[i].y = 0.0f;
      } else {
        points[i].x = 0.0f;
        points[i].y = SDL_randf() * ((float)WINDOW_HEIGHT);
      }
      point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
    }
  }
}