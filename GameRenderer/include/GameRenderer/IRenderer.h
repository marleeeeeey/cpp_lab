#pragma once

// Interface for the IRenderContainer class to operate
class IRenderer {
 public:
  virtual ~IRenderer() = default;
  virtual void render() = 0;
  void setVisible(bool flag) { isVisible_ = flag; }

 protected:
  bool isVisible_ = true;
};