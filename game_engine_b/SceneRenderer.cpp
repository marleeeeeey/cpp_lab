#include "SceneRenderer.h"

#include <SDL3/SDL.h>
#include <imgui/imgui.h>

#include "GameDataForRendering.h"

void SceneRenderer::init(SDL_Renderer* renderer) {
  assert(renderer);
  this->renderer = renderer;
}

void SceneRenderer::describeGameWorldScene(const GameDataForRendering& gameDataForRendering) {
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE); /* white, full alpha */
  /* You can also draw single points with SDL_RenderPoint(), but it's
     cheaper (sometimes significantly so) to do them all at once. */
  SDL_RenderPoints(renderer, /* draw all the points! */
                   gameDataForRendering.points.data(),
                   gameDataForRendering.points.size());
}

void SceneRenderer::describeImGuiFrame(const GameDataForRendering& gameDataForRendering) {
  // https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp
  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.

  static float f = 0.0f;
  static int counter = 0;
  static float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};

  ImGuiIO& io = ImGui::GetIO();

  ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!" and append into it.

  ImGui::Text("This is some useful text.");  // Display some text (you can use a format strings too)

  ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
  ImGui::ColorEdit3("clear color", (float*)&clear_color);  // Edit 3 floats representing a color

  if (ImGui::Button("Button"))  // Buttons return true when clicked (most widgets return true when edited/activated)
    counter++;
  ImGui::SameLine();
  ImGui::Text("counter = %d", counter);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
  ImGui::End();
}