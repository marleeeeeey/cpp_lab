#pragma once

#include <entt/signal/dispatcher.hpp>
#include <memory>

#include "ClientNetwork.h"
#include "GameRenderer/IChatRenderer.h"
#include "GameRenderer/IDebugRender.h"
#include "GameRenderer/IRenderContainer.h"
#include "GameUtils/GameTimer.h"
#include "SdlApp/ISdlApp.h"
#include "SdlApp/SdlWithImGuiWrapper.h"
#include "SnowflakesSimulation.h"
#include "UserInputManager/IUserInputManager.h"

// ------------------------------------
// Forward declarations
// ------------------------------------

class INetworkTransport;

// -----------------------------
// The main application object.
// ----------------------------

// ClientApp is a bridge between GameWorld, ClientNetwork, Render Classes, UserInputManager.
// It overrides ISdlApp interface and uses SdlWithImGuiWrapper as Helper.
class ClientApp : public ISdlApp {
 public:
  // ----------------------------------------------
  // SDL Based Steps (override ISdlApp interface)
  // ----------------------------------------------

  ~ClientApp() override;

  SDL_AppResult init(int argc, char* argv[]) override;
  SDL_AppResult onEvent(SDL_Event* event) override;
  SDL_AppResult iterate() override;

 private:
  // -------------------
  // Input and Events
  // -------------------

  void sendUserInputToServer_();
  std::unique_ptr<IUserInputManager> userInputManager_;
  entt::dispatcher dispatcher_;
  void onKeyPressed(const KeyPressed&);
  bool showDebugWindows_ = false;

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

  std::shared_ptr<SnowflakesSimulation> snowflakesSimulation_;
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

  void iterateGameWorld_(float elapsed);
  void iterateDebugRender_();

  // ---------
  // Network
  // ---------

  std::shared_ptr<ClientNetwork> gameNetwork_;
};