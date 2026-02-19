#include "RenderContainer.h"

#include <SDL3/SDL.h>
#include <imgui.h>

#include <algorithm>

#include "ChatRenderer.h"

RenderContainer::RenderContainer(SDL_Renderer* sdlRenderer) {
  assert(sdlRenderer);
  sdlRenderer_ = sdlRenderer;
}

void RenderContainer::addRenderer(std::weak_ptr<IRenderer> renderer) {
  renderers_.push_back(renderer);
}

void RenderContainer::render() {
  for (const auto& renderer : renderers_) {
    if (auto r = renderer.lock()) {
      r->render();
    }
  }
}

void RenderContainer::onWindowSizeChanged(int width, int height) {
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
}
