#pragma once

struct SDL_Renderer;
struct GameDataForRendering;

// Renders the game world and ImGui GUI. Called by AppInstance.
class SceneRenderer {
  SDL_Renderer* renderer = nullptr;

 public:
  void init(SDL_Renderer* renderer);
  void describeGameWorldScene(const GameDataForRendering& gameDataForRendering);
  void describeImGuiFrame(const GameDataForRendering& gameDataForRendering);
};
