#pragma once

struct GameDataForRendering {
  /* Points are plotted as a set of X and Y coordinates.
     (0, 0) is the top left of the window, and larger numbers go down
     and to the right. This isn't how geometry works, but this is pretty
     standard in 2D graphics. */
  std::vector<SDL_FPoint> points;
};
