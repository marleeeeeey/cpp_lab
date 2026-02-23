#pragma once
#include <memory>

#include "IRenderer.h"

// Interface for managing renderers.
// Allows adding renderers. Redner apply in the order they were added.
class IRenderContainer {
 public:
  // -------------
  // Factory
  // -------------
  static std::unique_ptr<IRenderContainer> create(void* renderer);
  virtual ~IRenderContainer() = default;

  // -------------
  // Interface
  // -------------

  virtual void addComponent(std::weak_ptr<IRenderer> component) = 0;
  virtual void render() = 0;
};
