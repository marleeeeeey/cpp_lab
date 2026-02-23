#pragma once
#include <memory>
#include <string>

#include "IRenderer.h"

class IDebugRender : public IRenderer {
 public:
  // ----------
  // Factory
  // ----------

  static std::unique_ptr<IDebugRender> create();
  virtual ~IDebugRender() = default;

  // -------------
  // Interface
  // -------------

  virtual void addLine(const std::string& line) = 0;
  virtual void render() = 0;
};