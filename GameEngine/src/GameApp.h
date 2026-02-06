#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "ChatDataForRendering.h"
#include "GameWorld.h"
#include "NetworkManager/INetworkManager.h"
#include "ReconnectPolicy.h"
#include "SceneRenderer.h"
#include "UserInputManger.h"

// ------------------------------------
// Forward declarations
// ------------------------------------

class ITransport;

// -----------------------------
// The main application object.
// ----------------------------

// It manages the SDL window and renderer, and the game world.
// GameApp is a bridge between GameWorld, SceneRenderer,
// UserInputManger and Network.
class GameApp {
  // ----------------
  // Technical Data
  // ----------------

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  Uint64 beginFrameTime_ = 0;
  ReconnectPolicy reconnect_;
  bool reconnectPending_ = false;
  bool connecting_ = false;

  // ------------------
  // Game Domain Data
  // ------------------

  GameWorld gameWorld_;
  SceneRenderer sceneRenderer_;
  UserInputManger userInputManger_;
  std::unique_ptr<INetworkManager> networkManager_;
  ChatDataForRendering chatDataForRendering_;

 public:
  // -----------------------------------
  // SDL Based Steps (public interface)
  // -----------------------------------

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

  // ---------------
  // Iterate Steps
  // ---------------

  float calculateDeltaTime_();
  void pollNetworkEvents_();
  GameDataForRendering updateGameWorld_(float elapsed);
  void renderFrame_(GameDataForRendering gameDataForRendering);
};