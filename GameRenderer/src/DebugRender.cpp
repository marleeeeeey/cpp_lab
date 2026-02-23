#include "DebugRender.h"

#include <imgui.h>

void DebugRender::addLine(const std::string& line) {
  lines_.push_back(line);
}

void DebugRender::render() {
  ImGui::SetNextWindowSize(ImVec2(520, 320), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowBgAlpha(0.05f);

  if (!ImGui::Begin("Debug", nullptr)) {
    ImGui::End();
    lines_.clear();
    return;
  }

  static char filter[256] = {0};

  ImGui::SetNextItemWidth(-1);
  ImGui::InputTextWithHint("##dbg_filter", "Filter...", filter, sizeof(filter));

  ImGui::Separator();

  ImGui::BeginChild("##dbg_lines",
                    ImVec2(0, 0),
                    ImGuiChildFlags_Borders,
                    ImGuiWindowFlags_HorizontalScrollbar);

  const bool hasFilter = filter[0] != '\0';

  for (const auto& line : lines_) {
    if (hasFilter) {
      if (line.find(filter) == std::string::npos) {
        continue;
      }
    }
    ImGui::TextUnformatted(line.c_str());
  }

  ImGui::EndChild();
  ImGui::End();

  lines_.clear();
}
