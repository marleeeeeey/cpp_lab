#include "RenderContainer.h"

#include <SDL3/SDL.h>
#include <imgui.h>

#include <algorithm>

#include "ChatRenderer.h"

RenderContainer::RenderContainer(SDL_Renderer* sdlRenderer) {
  assert(sdlRenderer);
  sdlRenderer_ = sdlRenderer;
}

void RenderContainer::addComponent(std::weak_ptr<IRenderer> component) {
  components_.push_back(component);
}

void RenderContainer::render() {
  for (const auto& renderer : components_) {
    if (auto r = renderer.lock()) {
      r->render();
    }
  }
}

void RenderContainer::onWindowSizeChanged(int width, int height) {
  ImGuiIO& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
}
