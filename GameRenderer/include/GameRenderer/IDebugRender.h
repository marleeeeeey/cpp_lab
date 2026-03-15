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

  // ------------------------
  // Interface IRenderer
  // ------------------------

  virtual void render(float dt, float gameTime) override = 0;

  // ------------------------
  // Interface IDebugRender
  // ------------------------

  // Add text line. Should be called every frame
  virtual void addLine(const std::string& line) = 0;

  // Add a text line that is always visible.
  virtual void addStaticLine(const std::string& key, const std::string& line) = 0;

  // Debug button always visible and may be configurable via this callback
  virtual void setOnDebugToggleCallback(std::function<void()> callback) = 0;

  // Add a button to the debug menu
  virtual void addButtonWithCallback(const std::string& label, std::function<void()> callback) = 0;

  // Add a checkbox to the debug menu. Disabled(false) by default
  virtual void addCheckboxWithCallback(const std::string& label, std::function<void(bool newValue)> callback) = 0;
};