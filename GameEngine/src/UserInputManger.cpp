#include "UserInputManger.h"

#include <spdlog/spdlog.h>

void UserInputManger::applyEvent(SDL_Event* event) {
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
}

const UserInputData& UserInputManger::getUserInputData() const {
  return userInputData_;
}

void UserInputManger::onFrameEnd() {
  userInputData_.pressed = {};
}