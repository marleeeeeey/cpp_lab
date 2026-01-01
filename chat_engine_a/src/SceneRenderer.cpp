#include "SceneRenderer.h"

#include <SDL3/SDL.h>
#include <imgui/imgui.h>

#include "DataForRendering.h"

void SceneRenderer::setRenderer(SDL_Renderer* renderer) {
  assert(renderer);
  this->renderer = renderer;
}

void SceneRenderer::renderGUI(const DataForRendering& dataForRendering) {
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGuiIO& io = ImGui::GetIO();
  ImGui::SetNextWindowSize(io.DisplaySize);

  ImGui::Begin("Chat Window", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);

  // Connection status
  ImGui::Text("Status: %s", dataForRendering.isConnected ? "Online" : "Offline");
  ImGui::Separator();

  // Message history
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
  for (const auto& msg : dataForRendering.chatHistory) {
    ImGui::TextWrapped("%s", msg.c_str());
  }
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);
  ImGui::EndChild();

  // Input field and send button
  static char inputBuf[256] = "";
  auto handleSend = [&]() {
    if (inputBuf[0] != '\0') {
      if (onMessageSent) {
        onMessageSent(inputBuf);
      }

      std::fill(std::begin(inputBuf), std::end(inputBuf), '\0');
      ImGui::SetKeyboardFocusHere(-1);
    }
  };
  if (ImGui::InputText("##Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
    handleSend();
  }
  ImGui::SameLine();
  if (ImGui::Button("Send")) {
    handleSend();
  }

  ImGui::End();
}

void SceneRenderer::setOnMessageSent(const OnMessageSentCallback& callback) { onMessageSent = callback; }