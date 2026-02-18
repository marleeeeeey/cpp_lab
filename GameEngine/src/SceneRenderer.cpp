#include "SceneRenderer.h"

#include <SDL3/SDL.h>
#include <imgui.h>

#include <algorithm>

#include "ChatDataForRendering.h"
#include "GameDataForRendering.h"
#include "TimeUtils.h"

void SceneRenderer::setRenderer(SDL_Renderer* renderer) {
  assert(renderer);
  this->renderer_ = renderer;
}

void SceneRenderer::onWindowSizeChanged(int width, int height) {
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
}

void SceneRenderer::renderGameObjects(const GameDataForRendering& gameDataForRendering) {
  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */

  for (int i = 0; i < gameDataForRendering.points.size(); i++) {
    const glm::vec2& point = gameDataForRendering.points[i];
    SDL_RenderPoint(renderer_, point.x, point.y);
  }
}

void SceneRenderer::renderChatWindow(const ChatDataForRendering& chatDataForRendering,
                                     const OnMessageSentCallback& onMessageSentCallback) {
  // ------------------------
  // Configure ImGui window
  // ------------------------

  ImGuiIO& io = ImGui::GetIO();

  // This window always occupies the entire screen of the host window
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
  ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);

  // Set up opacity to show the falling snow on the back
  ImGui::SetNextWindowBgAlpha(0.25f);

  // Disable window interaction
  const ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoTitleBar;

  // --------------
  // Start window
  // --------------

  ImGui::Begin("Chat Window", nullptr, flags);

  // -------------------
  // Connection status
  // -------------------

  ImGui::Text("Your Status: %s | Number Of Users: %d",
              chatDataForRendering.connectionStatus.c_str(), chatDataForRendering.numberOfConnectedUsers);
  ImGui::Separator();

  // ------------------
  // Message history
  // ------------------

  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()),
                    false, ImGuiWindowFlags_HorizontalScrollbar);
  for (const auto& chatMessage : chatDataForRendering.getChatHistory()) {
    ImGui::TextWrapped("[%s] %s [%d]: %s",
                       TimeUtils::makeTimePrefixHHMMSS(chatMessage.timestamp).c_str(),
                       chatMessage.sender.name.c_str(),
                       chatMessage.sender.messagesSent,
                       chatMessage.message.c_str());
  }
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);
  ImGui::EndChild();

  // -------------
  // Input field
  // -------------

  static char inputBuf[256] = "";
  auto handleSend = [&]() {
    if (inputBuf[0] != '\0') {
      if (onMessageSentCallback) {
        onMessageSentCallback(inputBuf);
      }

      std::fill(std::begin(inputBuf), std::end(inputBuf), '\0');
      ImGui::SetKeyboardFocusHere(-1);
    }
  };

  const float sendW = 90.0f;                                                                // Width of the "Send" button
  const float spacing = ImGui::GetStyle().ItemSpacing.x;                                    // Default spacing between elements
  const float inputW = std::max(1.0f, ImGui::GetContentRegionAvail().x - sendW - spacing);  // Width of the input field
  ImGui::SetNextItemWidth(inputW);

  if (ImGui::InputText("##Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
    handleSend();
  }

  // --------------------------
  // Send button on same line
  // --------------------------

  ImGui::SameLine();
  if (ImGui::Button("Send", ImVec2(sendW, ImGui::GetFrameHeight()))) {
    handleSend();
  }

  ImGui::End();
}