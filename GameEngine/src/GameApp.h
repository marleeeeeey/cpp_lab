#pragma once

#include <SDL3/SDL.h>

#include <entt/signal/sigh.hpp>
#include <memory>

#include "AutoReconnectionNetwork/IAutoReconnectionNetwork.h"
#include "ChatDataForRendering.h"
#include "GameWorld.h"
#include "NetworkDataHandler/INetworkDataHandler.h"
#include "SceneRenderer.h"
#include "UserInputManger.h"

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
  // ---------------------------
  // Rendering and window data
  // ---------------------------

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  Uint64 beginFrameTime_ = 0;
  int windowWidth_ = 800;
  int windowHeight_ = 600;
  ChatDataForRendering chatDataForRendering_;

  // ----------------------------------------------
  // Subscription data for window size changes
  // ----------------------------------------------

  entt::sigh<void(int width, int height)> onWindowSizeChangedSignal_;
  auto onWindowSizeChangedSink() { return entt::sink{onWindowSizeChangedSignal_}; }

  // ------------------
  // Game Domain Data
  // ------------------

  GameWorld gameWorld_;
  SceneRenderer sceneRenderer_;
  UserInputManger userInputManger_;

 public:
  // ------------------------------------
  // SDL Based Steps (public interface)
  // ------------------------------------

  SDL_AppResult init(int argc, char* argv[]);
  SDL_AppResult onEvent(SDL_Event* event);
  SDL_AppResult iterate();
  void onQuit();

 private:
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
  SDL_AppResult initSDL_();
  void initImGui_();
  void initRenderer_();
  void initGameWorld_();
  void initNetworkHandlers_();

  // ----------------------
  // Basic Iterate Steps
  // ----------------------

  float calculateDeltaTime_();
  GameDataForRendering updateGameWorld_(float elapsed);
  void renderFrame_(GameDataForRendering gameDataForRendering);

  // ---------
  // Network
  // ---------

  std::unique_ptr<IAutoReconnectionNetwork> autoReconnectionNetwork_;
  std::unique_ptr<INetworkDataHandler> networkDataHandler_;
};