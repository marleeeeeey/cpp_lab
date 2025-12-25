#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL_main.h>
#include "AppInstance.h"

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
  AppInstance* app = new AppInstance();
  auto result = app->init();
  *appstate = app;
  return result;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
  return ((AppInstance*)appstate)->onEvent(event);
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void* appstate) {
  return ((AppInstance*)appstate)->onTick();
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
  ((AppInstance*)appstate)->onQuit();
  /* SDL will clean up the window/renderer for us. */
}
