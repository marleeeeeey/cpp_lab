#include "SceneRenderer.h"

#include <SDL3/SDL.h>
#include <imgui.h>

#include <algorithm>

#include "ChatDataForRendering.h"
#include "GameDataForRendering.h"

void SceneRenderer::setRenderer(SDL_Renderer* renderer) {
  assert(renderer);
  this->renderer_ = renderer;
}

void SceneRenderer::renderGameObjects(const GameDataForRendering& gameDataForRendering) {
  SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */

  for (int i = 0; i < gameDataForRendering.points.size(); i++) {
    const glm::vec2& point = gameDataForRendering.points[i];
    SDL_RenderPoint(renderer_, point.x, point.y);
  }
}

void SceneRenderer::renderHelloWorldWindow() {
  // -------------------------------------------------------------
  // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
  // Show a simple window that we create ourselves.
  // We use a Begin/End pair to create a named window.
  // -------------------------------------------------------------

  static float f = 0.0f;
  static int counter = 0;
  static float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};

  ImGuiIO& io = ImGui::GetIO();

  // ------------------------------------------------------------
  // Create a window called "Hello, world!" and append into it.
  // ------------------------------------------------------------
  ImGui::Begin("Hello, world!");

  ImGui::Text("This is some useful text.");  // Display some text (you can use a format strings too)

  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::ColorEdit3("clear color", (float*)&clear_color);  // Edit 3 floats representing a color

  // Buttons return true when clicked (most widgets return true when edited/activated)
  if (ImGui::Button("Button")) {
    counter++;
  }
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

  ImGui::End();
}

void SceneRenderer::renderChatWindow(const ChatDataForRendering& chatDataForRendering,
                                     const OnMessageSentCallback& onMessageSentCallback) {
  // --------------------------------------------------
  // Create window "Chat history and new message sent"
  // --------------------------------------------------

  ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowPos(ImVec2(15, 15), ImGuiCond_FirstUseEver);

  ImGui::Begin("Chat Window");

  // Connection status
  ImGui::Text("Status: %s", chatDataForRendering.isConnected ? "Online" : "Offline");
  ImGui::Separator();

  // Message history
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
  for (const auto& msg : chatDataForRendering.getChatHistory()) {
    ImGui::TextWrapped("%s", msg.c_str());
  }
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    ImGui::SetScrollHereY(1.0f);
  ImGui::EndChild();

  // Input field and send button
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
  if (ImGui::InputText("##Input", inputBuf, IM_ARRAYSIZE(inputBuf), ImGuiInputTextFlags_EnterReturnsTrue)) {
    handleSend();
  }
  ImGui::SameLine();
  if (ImGui::Button("Send")) {
    handleSend();
  }

  ImGui::End();
}