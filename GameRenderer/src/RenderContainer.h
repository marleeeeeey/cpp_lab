#pragma once
#include <memory>
#include <vector>

#include "GameRenderer/IRenderContainer.h"

// ---------------------
// Forward Declarations
// ---------------------

struct SDL_Renderer;

// ----------------------
// RenderContainer class
// ----------------------

class RenderContainer : public IRenderContainer {
 public:
  explicit RenderContainer(SDL_Renderer* sdlRenderer);
  void addComponent(std::weak_ptr<IRenderer> component) override;
  void render(float dt, float gameTime) override;

 private:
  SDL_Renderer* sdlRenderer_ = nullptr;
  std::vector<std::weak_ptr<IRenderer>> components_;
};
