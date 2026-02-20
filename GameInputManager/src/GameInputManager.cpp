#include "GameInputManager.h"

#include <spdlog/spdlog.h>

GameInputManager::GameInputManager(SDL_Window* window, int windowWidth, int windowHeight) {
  window_ = window;
  windowWidth_ = windowWidth;
  windowHeight_ = windowHeight;
}

SDL_AppResult GameInputManager::applyEvent(SDL_Event* event) {
  if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);
    onWindowSizeChangedSignal_.publish(windowWidth_, windowHeight_);
  }

  if (event->type == SDL_EVENT_QUIT) {
    // end the program, reporting success to the OS
    return SDL_APP_SUCCESS;
  }

  if (event->type == SDL_EVENT_KEY_DOWN) {
    SPDLOG_DEBUG("Key pressed: {}", event->key.key);
  }

  if (event->type == SDL_EVENT_KEY_UP) {
    SPDLOG_DEBUG("Key released: {}", event->key.key);
  }

  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      if (event->key.repeat)
        break;  // ignore repeating keys
      switch (event->key.key) {
        case SDLK_UP:
          userInputData_.pressed.up = true;
          userInputData_.held.up = true;
          break;
        case SDLK_DOWN:
          userInputData_.pressed.down = true;
          userInputData_.held.down = true;
          break;
        case SDLK_LEFT:
          userInputData_.pressed.left = true;
          userInputData_.held.left = true;
          break;
        case SDLK_RIGHT:
          userInputData_.pressed.right = true;
          userInputData_.held.right = true;
          break;
      }
      break;
    case SDL_EVENT_KEY_UP:
      switch (event->key.key) {
        case SDLK_UP:
          userInputData_.held.up = false;
          break;
        case SDLK_DOWN:
          userInputData_.held.down = false;
          break;
        case SDLK_LEFT:
          userInputData_.held.left = false;
          break;
        case SDLK_RIGHT:
          userInputData_.held.right = false;
          break;
      }
      break;
  }

  // OK status - app should continue
  return SDL_APP_CONTINUE;
}

const GameInputData& GameInputManager::getGameInputData() const {
  return userInputData_;
}

void GameInputManager::onFrameEnd() {
  userInputData_.pressed = {};
}

void GameInputManager::onAppQuit() {
  onWindowSizeChangedSink().disconnect();
}