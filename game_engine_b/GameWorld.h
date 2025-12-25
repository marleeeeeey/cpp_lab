#pragma once
#include <SDL3/SDL.h>

#include <vector>

struct GameWorld {
  /* (track everything as parallel arrays instead of a array of structs,
     so we can pass the coordinates to the renderer in a single function call.) */

  /* Points are plotted as a set of X and Y coordinates.
     (0, 0) is the top left of the window, and larger numbers go down
     and to the right. This isn't how geometry works, but this is pretty
     standard in 2D graphics. */
  std::vector<SDL_FPoint> points;
  std::vector<float> point_speeds;

  void init();

  void iterate(double elapsed);

  std::vector<SDL_FPoint>& getRenderData() {
    return points;
  }
};
