#pragma once

#include <memory>

#include "GameInputManager/IGameInputManager.h"
#include "GameNetwork.h"
#include "GameRenderer/IChatRenderer.h"
#include "GameRenderer/IDebugRender.h"
#include "GameRenderer/IRenderContainer.h"
#include "GameTimer.h"
#include "GameWorld.h"
#include "SdlApp/ISdlApp.h"
#include "SdlApp/SdlWithImGuiWrapper.h"

// ------------------------------------
// Forward declarations
// ------------------------------------

class INetworkTransport;

// -----------------------------
// The main application object.
// ----------------------------

// GameApp is a bridge between GameWorld, GameNetwork, Render Classes, GameInputManager.
// It overrides ISdlApp interface and uses SdlWithImGuiWrapper as Helper.
class GameApp : public ISdlApp {
 public:
  // ----------------------------------------------
  // SDL Based Steps (override ISdlApp interface)
  // ----------------------------------------------

  ~GameApp() override;

  SDL_AppResult init(int argc, char* argv[]) override;
  SDL_AppResult onEvent(SDL_Event* event) override;
  SDL_AppResult iterate() override;

 private:
  // ---------------------------
  // Rendering and window data
  // ---------------------------

  std::unique_ptr<SdlWithImGuiWrapper> sdlWrapper_;
  std::shared_ptr<IGameWorldRenderer> gameWorldRenderer_;
  std::shared_ptr<IChatRenderer> chatRenderer_;
  std::shared_ptr<IDebugRender> debugRender_;
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
  void updateDebugRender_();

  // ---------
  // Network
  // ---------

  std::shared_ptr<GameNetwork> gameNetwork_;
};