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
  void addRenderer(std::weak_ptr<IRenderer> renderer) override;
  void render() override;
  void onWindowSizeChanged(int width, int height) override;

 private:
  SDL_Renderer* sdlRenderer_ = nullptr;
  std::vector<std::weak_ptr<IRenderer>> renderers_;
};
