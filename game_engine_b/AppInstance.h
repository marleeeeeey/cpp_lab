#pragma once

#include <SDL3/SDL.h>

class AppInstance {
  /* We will use this renderer to draw into this window every frame. */
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;
  Uint64 last_time = 0;
  static constexpr int WINDOW_WIDTH = 640;
  static constexpr int WINDOW_HEIGHT = 480;
  static constexpr int NUM_POINTS = 500;
  static constexpr int MIN_PIXELS_PER_SECOND = 30; /* move at least this many pixels per second. */
  static constexpr int MAX_PIXELS_PER_SECOND = 60; /* move this many pixels per second at most. */

  /* (track everything as parallel arrays instead of a array of structs,
     so we can pass the coordinates to the renderer in a single function call.) */

  /* Points are plotted as a set of X and Y coordinates.
     (0, 0) is the top left of the window, and larger numbers go down
     and to the right. This isn't how geometry works, but this is pretty
     standard in 2D graphics. */
  SDL_FPoint points[NUM_POINTS];
  float point_speeds[NUM_POINTS];

 public:
  SDL_AppResult init();
  SDL_AppResult onEvent(SDL_Event* event);
  SDL_AppResult onTick();
  void onQuit();
};