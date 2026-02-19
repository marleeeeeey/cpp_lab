#pragma once

// Interface for the IRenderContainer class to operate
class IRenderer {
 public:
  virtual ~IRenderer() = default;
  virtual void render() = 0;
};