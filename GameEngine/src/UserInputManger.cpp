#include "UserInputManger.h"

void UserInputManger::applyEvent(SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      if (event->key.repeat)
        break;  // ignore repeating keys
      switch (event->key.key) {
        case SDLK_UP:
          userInputData.pressed.up = true;
          userInputData.held.up = true;
          break;
        case SDLK_DOWN:
          userInputData.pressed.down = true;
          userInputData.held.down = true;
          break;
        case SDLK_LEFT:
          userInputData.pressed.left = true;
          userInputData.held.left = true;
          break;
        case SDLK_RIGHT:
          userInputData.pressed.right = true;
          userInputData.held.right = true;
          break;
      }
      break;
    case SDL_EVENT_KEY_UP:
      switch (event->key.key) {
        case SDLK_UP:
          userInputData.held.up = false;
          break;
        case SDLK_DOWN:
          userInputData.held.down = false;
          break;
        case SDLK_LEFT:
          userInputData.held.left = false;
          break;
        case SDLK_RIGHT:
          userInputData.held.right = false;
          break;
      }
      break;
  }
}

const UserInputData& UserInputManger::getUserInputData() const {
  return userInputData;
}

void UserInputManger::onFrameEnd() {
  userInputData.pressed = {};
}