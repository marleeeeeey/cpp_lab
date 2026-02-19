#pragma once
#include <memory>

#include "IRenderer.h"

// Interface for managing renderers.
// Allows adding renderers. Redner apply in the order they were added.
class IRenderContainer {
 public:
  static std::unique_ptr<IRenderContainer> create(void* renderer);
  virtual ~IRenderContainer() = default;
  virtual void addRenderer(std::weak_ptr<IRenderer> renderer) = 0;
  virtual void render() = 0;
  virtual void onWindowSizeChanged(int width, int height) = 0;
};
