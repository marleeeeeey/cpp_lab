#include "SceneRenderer.h"

#include <SDL3/SDL.h>
#include <imgui.h>

#include <algorithm>

#include "ChatRenderer.h"
#include "TimeUtils.h"

void SceneRenderer::addRenderer(std::shared_ptr<IRenderer> renderer) {
  renderers_.push_back(renderer);
}

void SceneRenderer::render() {
  for (const auto& renderer : renderers_) {
    if (auto r = renderer.lock()) {
      r->render();
    }
  }
}

void SceneRenderer::setSdlRenderer(SDL_Renderer* sdlRenderer) {
  assert(sdlRenderer);
  this->sdlRenderer_ = sdlRenderer;
}

void SceneRenderer::onWindowSizeChanged(int width, int height) {
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
}
