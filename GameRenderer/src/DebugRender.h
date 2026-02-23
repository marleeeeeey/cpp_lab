#pragma once
#include <string>
#include <vector>

#include "GameRenderer/IDebugRender.h"

class DebugRender : public IDebugRender {
 public:
  void addLine(const std::string& line);
  void render();

 private:
  std::vector<std::string> lines_;
};
