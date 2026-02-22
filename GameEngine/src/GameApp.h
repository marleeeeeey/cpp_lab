#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "GameInputManager/IGameInputManager.h"
#include "GameNetwork.h"
#include "GameRenderer/IChatRenderer.h"
#include "GameRenderer/IRenderContainer.h"
#include "GameTimer.h"
#include "GameWorld.h"
#include "SDL_IMGUI_Wrapper.h"

// ------------------------------------
// Forward declarations
// ------------------------------------

class INetworkTransport;

// -----------------------------
// The main application object.
// ----------------------------

// It manages the SDL window and renderer, and the game world.
// GameApp is a bridge between GameWorld, SceneRenderer,
// UserInputManger and Network.
class GameApp {
 public:
  // ------------------------------------
  // SDL Based Steps (public interface)
  // ------------------------------------

  SDL_AppResult init(int argc, char* argv[]);
  SDL_AppResult onEvent(SDL_Event* event);
  SDL_AppResult iterate();
  void onQuit();

 private:
  // ---------------------------
  // Rendering and window data
  // ---------------------------

  std::unique_ptr<SDL_IMGUI_Wrapper> sdlImGuiWrapper_;
  std::shared_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::shared_ptr<IChatRenderer> chatRenderer_;
  std::shared_ptr<IRenderContainer> renderContainer_;

  // ------------------
  // Game Domain Data
  // ------------------

  std::shared_ptr<GameWorld> gameWorld_;
  std::unique_ptr<IGameInputManager> gameInputManager_;
  std::shared_ptr<GameTimer> gameTimer_;

  // ---------------------
  // Application Options
  // ---------------------

  struct Options {
    std::string url;
  } appOptions_;

  // ------------
  // Init Steps
  // ------------

  void initTracyProfiler_();
  void initOptions_(int argc, char* argv[]);
  void initGameWorld_();
  void initChat_();
  void initTimers_();

  // ----------------------
  // Basic Iterate Steps
  // ----------------------

  void updateGameWorld_(float elapsed);
  void renderFrame_();

  // ---------
  // Network
  // ---------

  std::shared_ptr<GameNetwork> gameNetwork_;
};