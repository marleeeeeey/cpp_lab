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
  if (ImGui::InputText("##Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
    // Called when Enter is pressed
    // TODO: add message to chat history in ChatManager
    inputBuf[0] = '\0';
  }
  ImGui::SameLine();
  if (ImGui::Button("Send")) {
    // TODO: add message to chat history in ChatManager
    inputBuf[0] = '\0';
  }

  ImGui::End();
}