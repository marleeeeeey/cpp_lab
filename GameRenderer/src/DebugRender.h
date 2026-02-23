#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "GameRenderer/IDebugRender.h"

class DebugRender : public IDebugRender {
 public:
  void addLine(const std::string& line) override;
  void addStaticLine(const std::string& key, const std::string& line) override;
  void render() override;

 private:
  std::vector<std::string> lines_;
  std::unordered_map<std::string, std::string> staticLines_;
};
