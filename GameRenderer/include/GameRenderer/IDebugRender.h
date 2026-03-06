#pragma once
#include <functional>
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
  virtual void addStaticLine(const std::string& key, const std::string& line) = 0;
  virtual void render(float dt, float gameTime) override = 0;
  virtual void setOnDebugToggleCallback(std::function<void()> callback) = 0;
  virtual void addButtonWithCallback(const std::string& label, std::function<void()> callback) = 0;
};