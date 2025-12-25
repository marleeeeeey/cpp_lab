#pragma once

struct SDL_Renderer;
struct GameDataForRendering;

// Renders the game world and ImGui GUI. Called by AppInstance.
class SceneRenderer {
  SDL_Renderer* renderer = nullptr;

 public:
  void setRenderer(SDL_Renderer* renderer);
  void render(const GameDataForRendering& gameDataForRendering);

 private:
  void renderGameObjects(const GameDataForRendering& gameDataForRendering);
  void renderGUI(const GameDataForRendering& gameDataForRendering);
};
