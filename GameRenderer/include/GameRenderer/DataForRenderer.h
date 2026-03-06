#pragma once

#include <optional>

#include "GameShared/WorldSnapshot.h"

struct DataForRenderer {
  WorldSnapshot snapshot;
  // Uses for local simulation and then correct by server data (interpolation).
  // Interpolation performs in RenderComponent.
  std::optional<Player> localPlayer;
};
