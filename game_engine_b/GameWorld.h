#pragma once
#include <SDL3/SDL.h>

#include <vector>

#include "GameDataForRendering.h"
#include "UserInputData.h"

class GameWorld {
  /* (track everything as parallel arrays instead of a array of structs,
     so we can pass the coordinates to the renderer in a single function call.) */

  GameDataForRendering gameDataForRendering;
  std::vector<float> point_speeds;

 public:
  void init();
  void iterate(double elapsed, const UserInputData& userInputData);
  const GameDataForRendering& getGameDataForRendering() const;
};
