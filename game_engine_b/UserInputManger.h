#pragma once
#include <SDL3/SDL_events.h>

#include "UserInputData.h"

// Reads SDL events and converts them into UserInputData.
class UserInputManger {
  UserInputData userInputData;

 public:
  void applyEvent(SDL_Event* event);
  const UserInputData& getUserInputData() const;
};
