#include "AppInstance.h"

SDL_AppResult AppInstance::init() {
  int i;

  SDL_SetAppMetadata("Example Renderer Points", "1.0", "com.example.renderer-points");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("examples/renderer/points", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

  /* set up the data for a bunch of points. */
  for (i = 0; i < SDL_arraysize(points); i++) {
    points[i].x = SDL_randf() * ((float)WINDOW_WIDTH);
    points[i].y = SDL_randf() * ((float)WINDOW_HEIGHT);
    point_speeds[i] = MIN_PIXELS_PER_SECOND + (SDL_randf() * (MAX_PIXELS_PER_SECOND - MIN_PIXELS_PER_SECOND));
  }

  last_time = SDL_GetTicks();

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult AppInstance::onEvent(SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult AppInstance::onTick() {
  const Uint64 now = SDL_GetTicks();
  const float elapsed = ((float)(now - last_time)) / 1000.0f; /* seconds since last iteration */
  int i;

  /* let's move all our points a little for a new frame. */
  for (i = 0; i < SDL_arraysize(points); i++) {
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

  last_time = now;

  /* as you can see from this, rendering draws over whatever was drawn before it. */
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);       /* black, full alpha */
  SDL_RenderClear(renderer);                                         /* start with a blank canvas. */
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
  SDL_RenderPoints(renderer, points, SDL_arraysize(points));         /* draw all the points! */

  /* You can also draw single points with SDL_RenderPoint(), but it's
     cheaper (sometimes significantly so) to do them all at once. */

  SDL_RenderPresent(renderer); /* put it all on the screen! */

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

void AppInstance::onQuit() {
}