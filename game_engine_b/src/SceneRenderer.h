#pragma once

// ---------------------
// Forward Declarations
// ---------------------

struct SDL_Renderer;
struct GameDataForRendering;
struct ChatDataForRendering;

// ----------------------
// SceneRenderer class
// ----------------------

// Renders the game world and ImGui GUI. Called by AppInstance.
class SceneRenderer {
  SDL_Renderer* renderer = nullptr;

 public:
  void setRenderer(SDL_Renderer* renderer);
  void render(const GameDataForRendering& gameDataForRendering, const ChatDataForRendering& chatDataForRendering);

 private:
  void renderGameObjects(const GameDataForRendering& gameDataForRendering);
  void renderGUI(const GameDataForRendering& gameDataForRendering, const ChatDataForRendering& chatDataForRendering);
};
