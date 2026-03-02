#include "DebugRender.h"

#include <imgui.h>

#include "GameUtils/GameUtils.h"

void DebugRender::addLine(const std::string& line) {
  lines_.push_back(line);
}

void DebugRender::addStaticLine(const std::string& key, const std::string& line) {
  staticLines_[key] = line;
}

void DebugRender::render() {
  // Clear dynamic lines anyway
  auto guard = makeScopeGuard([this] { lines_.clear(); });

  // ------------------------
  // Configure ImGui window
  // ------------------------

  ImGuiIO& io = ImGui::GetIO();

  ImVec2 halfDisplaySize = io.DisplaySize;
  halfDisplaySize.x /= 2;
  ImGui::SetNextWindowPos(ImVec2{halfDisplaySize.x, 0}, ImGuiCond_Always);
  ImGui::SetNextWindowSize(halfDisplaySize, ImGuiCond_Always);
  ImGui::SetNextWindowBgAlpha(0.0f);

  // Disable window interaction
  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoTitleBar;

  // -----------------------
  // Render ImGui window
  // -----------------------

  if (!ImGui::Begin("Debug", nullptr, flags)) {
    ImGui::End();
    lines_.clear();
    return;
  }

  static char filter[256] = {0};

  const float buttonW = 90.0f;
  const float spacing = ImGui::GetStyle().ItemSpacing.x;
  const float inputW = std::max(1.0f, ImGui::GetContentRegionAvail().x - buttonW - spacing);

  if (isVisible_) {
    ImGui::SetNextItemWidth(inputW);
    ImGui::InputTextWithHint("##dbg_filter", "Filter...", filter, sizeof(filter));
  } else {
    ImGui::Dummy(ImVec2(inputW, ImGui::GetFrameHeight()));
  }

  ImGui::SameLine();
  if (ImGui::Button("DEBUG", ImVec2(buttonW, ImGui::GetFrameHeight()))) {
    if (onDebugToggleCallback_) {
      onDebugToggleCallback_();
    }
  }

  // ----------------------
  // Draw Debug Console
  // ----------------------

  if (isVisible_) {
    ImGui::BeginChild("##dbg_lines",
                      ImVec2(0, 0),
                      ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_HorizontalScrollbar);

    const bool hasFilter = filter[0] != '\0';

    for (const auto& [key, line] : staticLines_) {
      if (hasFilter) {
        if (line.find(filter) == std::string::npos) {
          continue;
        }
      }
      ImGui::TextUnformatted(line.c_str());
    }

    for (const auto& line : lines_) {
      if (hasFilter) {
        if (line.find(filter) == std::string::npos) {
          continue;
        }
      }
      ImGui::TextUnformatted(line.c_str());
    }

    ImGui::EndChild();
  }

  // ----------------------------
  // Complete Window Rendering
  // ----------------------------

  ImGui::End();
}

void DebugRender::setOnDebugToggleCallback(std::function<void()> callback) {
  onDebugToggleCallback_ = callback;
}
