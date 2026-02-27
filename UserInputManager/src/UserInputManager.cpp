#include "UserInputManager.h"

#include <spdlog/spdlog.h>

UserInputManager::UserInputManager(entt::dispatcher& dispatcher)
    : dispatcher_(dispatcher) {}

SDL_AppResult UserInputManager::applyEvent(SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    // end the program, reporting success to the OS
    return SDL_APP_SUCCESS;
  }

  checkKeyboardInputForDispatcher_(event);
  checkKeyboardInput_(event);
  checkMouseInput_(event);

  // OK status - app should continue
  return SDL_APP_CONTINUE;
}

const UserInputData& UserInputManager::getUserInputData() const {
  return userInputData_;
}

void UserInputManager::onFrameEnd() {
  userInputData_.keyboard.pressed = {};
  userInputData_.mouse.dx = 0;
  userInputData_.mouse.dy = 0;
  userInputData_.mouse.wheelX = 0.0f;
  userInputData_.mouse.wheelY = 0.0f;
  userInputData_.mouse.pressed = {};
  userInputData_.mouse.released = {};
}

// Uses SDL_Scancode type. It is physical representation of key
void UserInputManager::checkKeyboardInputForDispatcher_(SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN: {
      dispatcher_.trigger(KeyPressed{event->key.scancode, event->key.repeat});
      heldScancodesForDispatcher_[event->key.scancode] = true;
      break;
    }
    case SDL_EVENT_KEY_UP: {
      dispatcher_.trigger(KeyReleased{event->key.scancode});
      heldScancodesForDispatcher_[event->key.scancode] = false;
      break;
    }
    default:
      break;
  }

  for (auto sc = 0; sc < heldScancodesForDispatcher_.size(); ++sc) {
    if (heldScancodesForDispatcher_[sc]) {
      dispatcher_.trigger(KeyHeld{static_cast<SDL_Scancode>(sc)});
    }
  }
}

// Uses SDL_Scancode type. It is physical representation of key
void UserInputManager::checkKeyboardInput_(SDL_Event* event) {
  switch (event->type) {
    case SDL_EVENT_KEY_DOWN:
      if (event->key.repeat)
        break;  // ignore repeating keys
      switch (event->key.scancode) {
        case SDL_SCANCODE_UP:
          userInputData_.keyboard.pressed.up = true;
          userInputData_.keyboard.held.up = true;
          break;
        case SDL_SCANCODE_DOWN:
          userInputData_.keyboard.pressed.down = true;
          userInputData_.keyboard.held.down = true;
          break;
        case SDL_SCANCODE_LEFT:
          userInputData_.keyboard.pressed.left = true;
          userInputData_.keyboard.held.left = true;
          break;
        case SDL_SCANCODE_RIGHT:
          userInputData_.keyboard.pressed.right = true;
          userInputData_.keyboard.held.right = true;
          break;
      }
      break;
    case SDL_EVENT_KEY_UP:
      switch (event->key.scancode) {
        case SDL_SCANCODE_UP:
          userInputData_.keyboard.held.up = false;
          userInputData_.keyboard.released.up = true;
          break;
        case SDL_SCANCODE_DOWN:
          userInputData_.keyboard.held.down = false;
          userInputData_.keyboard.released.down = true;
          break;
        case SDL_SCANCODE_LEFT:
          userInputData_.keyboard.held.left = false;
          userInputData_.keyboard.released.left = true;
          break;
        case SDL_SCANCODE_RIGHT:
          userInputData_.keyboard.held.right = false;
          userInputData_.keyboard.released.right = true;
          break;
      }
      break;
  }
}

void UserInputManager::checkMouseInput_(SDL_Event* event) {
  // -----------------------------------------------
  // Global screen position (desktop coordinates)
  // -----------------------------------------------
  float gx = 0.0f, gy = 0.0f;
  SDL_GetGlobalMouseState(&gx, &gy);
  userInputData_.mouse.screenX = static_cast<int>(gx);
  userInputData_.mouse.screenY = static_cast<int>(gy);

  switch (event->type) {
      // ------------------------------------
      // Window-relative mouse position
      // ------------------------------------

    case SDL_EVENT_MOUSE_MOTION: {
      // SDL3 typically provides x/y and xrel/yrel (relative motion since last event)
      userInputData_.mouse.winX = static_cast<int>(event->motion.x);
      userInputData_.mouse.winY = static_cast<int>(event->motion.y);

      // Accumulate deltas during the frame
      userInputData_.mouse.dx += static_cast<int>(event->motion.xrel);
      userInputData_.mouse.dy += static_cast<int>(event->motion.yrel);
      break;
    }

      // --------------
      // Mouse wheel
      // --------------

    case SDL_EVENT_MOUSE_WHEEL: {
      // Wheel is an impulse: accumulate for this frame, then reset each frame
      userInputData_.mouse.wheelX += event->wheel.x;
      userInputData_.mouse.wheelY += event->wheel.y;
      break;
    }

      // ---------------
      // Mouse buttons
      // ---------------

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      const bool down = (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN);

      auto setButton = [&](auto& held, auto& pressed, auto& released) {
        if (down) {
          if (!held) {
            // prevent "pressed" from repeating if already held
            pressed = true;
          }
          held = true;
        } else {
          if (held) {
            released = true;
          }
          held = false;
        }
      };

      switch (event->button.button) {
        case SDL_BUTTON_LEFT:
          setButton(userInputData_.mouse.held.left,
                    userInputData_.mouse.pressed.left,
                    userInputData_.mouse.released.left);
          break;
        case SDL_BUTTON_RIGHT:
          setButton(userInputData_.mouse.held.right,
                    userInputData_.mouse.pressed.right,
                    userInputData_.mouse.released.right);
          break;
        case SDL_BUTTON_MIDDLE:
          setButton(userInputData_.mouse.held.middle,
                    userInputData_.mouse.pressed.middle,
                    userInputData_.mouse.released.middle);
          break;
        case SDL_BUTTON_X1:
          setButton(userInputData_.mouse.held.x1,
                    userInputData_.mouse.pressed.x1,
                    userInputData_.mouse.released.x1);
          break;
        case SDL_BUTTON_X2:
          setButton(userInputData_.mouse.held.x2,
                    userInputData_.mouse.pressed.x2,
                    userInputData_.mouse.released.x2);
          break;
      }
      break;
    }
  }
}
