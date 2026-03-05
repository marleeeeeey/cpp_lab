#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "GameRenderer/IDebugRender.h"

class DebugRender : public IDebugRender {
 public:
  void addLine(const std::string& line) override;
  void addStaticLine(const std::string& key, const std::string& line) override;
  void render() override;
  void setOnDebugToggleCallback(std::function<void()> callback) override;
  void addButtonWithCallback(const std::string& label, std::function<void()> callback) override;

 private:
  std::vector<std::string> lines_;
  std::unordered_map<std::string, std::string> staticLines_;
  std::function<void()> onDebugToggleCallback_;
  std::map<std::string, std::function<void()>> buttonCallbacks_;
};
