#pragma once

struct SDL_Renderer;
struct DataForRendering;

// Renders GUI for chat.
class SceneRenderer {
  SDL_Renderer* renderer = nullptr;

 public:
  void setRenderer(SDL_Renderer* renderer);
  void renderGUI(const DataForRendering& dataForRendering);
};
