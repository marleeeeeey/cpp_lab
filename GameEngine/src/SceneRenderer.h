#pragma once
#include <memory>
#include <vector>

#include "IRenderer.h"

// ---------------------
// Forward Declarations
// ---------------------

struct SDL_Renderer;

// ----------------------
// SceneRenderer class
// ----------------------

// Renders the game world and ImGui GUI. Called by AppInstance.
class SceneRenderer {
  SDL_Renderer* sdlRenderer_ = nullptr;
  std::vector<std::weak_ptr<IRenderer>> renderers_;

 public:
  void setSdlRenderer(SDL_Renderer* sdlRenderer);
  void addRenderer(std::shared_ptr<IRenderer> renderer);
  void render();
  void onWindowSizeChanged(int width, int height);
};
