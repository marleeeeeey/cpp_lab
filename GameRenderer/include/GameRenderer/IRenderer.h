#pragma once
#include "GlobalTypes/GlobalTypes.h"

// Interface for the IRenderContainer class to operate
class IRenderer {
 public:
  virtual ~IRenderer() = default;
  virtual void render(float dt, float gameTime) = 0;
  void setVisible(bool flag) { isVisible_ = flag; }
  void setStaticDebugCb(StaticDebugCb cb) { staticDebugCb_ = cb; }

 protected:
  void safeStaticDebug(const std::string& key, const std::string& value) const {
    if (staticDebugCb_) {
      staticDebugCb_(key, value);
    }
  }
  bool isVisible_ = true;

 private:
  StaticDebugCb staticDebugCb_;
};