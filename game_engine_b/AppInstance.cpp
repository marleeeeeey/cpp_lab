#include "AppInstance.h"

#include "GlobalConstants.h"

SDL_AppResult AppInstance::init() {
  SDL_SetAppMetadata("Example Renderer Points", "1.0", "com.example.renderer-points");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("examples/renderer/points",
                                   WINDOW_WIDTH, WINDOW_HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

  gameWorld.init();

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

  UserInputData userInputData;
  gameWorld.iterate(elapsed, userInputData);
  GameDataForRendering gameDataForRendering = gameWorld.getGameDataForRendering();

  last_time = now;

  /* as you can see from this, rendering draws over whatever was drawn before it. */
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);       /* black, full alpha */
  SDL_RenderClear(renderer);                                         /* start with a blank canvas. */
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
  SDL_RenderPoints(renderer,                                         /* draw all the points! */
                   gameDataForRendering.points.data(),
                   gameDataForRendering.points.size());

  /* You can also draw single points with SDL_RenderPoint(), but it's
     cheaper (sometimes significantly so) to do them all at once. */

  SDL_RenderPresent(renderer); /* put it all on the screen! */

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

void AppInstance::onQuit() {
  // ...
  /* SDL will clean up the window/renderer for us. */
}