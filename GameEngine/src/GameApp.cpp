#include "GameApp.h"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <cxxopts.hpp>

#include "GameSharedObjects/ChatMessage.h"
#include "Profiler/Profiler.h"

// TODO: remove initial values duplication from here
constexpr int INITIAL_WINDOWS_WIDTH = 800;
constexpr int INITIAL_WINDOWS_HEIGHT = 600;

// ------------------
// SDL Based Steps
// ------------------

SDL_AppResult GameApp::init(int argc, char* argv[]) {
  // ---------------------------
  // Init Auxiliary Components
  // ---------------------------

  // spdlog::set_level(spdlog::level::trace);
  initOptions_(argc, argv);
  initTracyProfiler_();

  // -----------------------------------
  // Init SDL, IMGui and InputManager
  // -----------------------------------

  sdlImGuiWrapper_ = std::make_unique<SDL_IMGUI_Wrapper>();
  renderContainer_ = IRenderContainer::create(sdlImGuiWrapper_->getRenderer());
  gameInputManager_ = IGameInputManager::create(sdlImGuiWrapper_->getWindow(), INITIAL_WINDOWS_WIDTH, INITIAL_WINDOWS_HEIGHT);
  gameInputManager_->onWindowSizeChangedSink().connect<&IRenderContainer::onWindowSizeChanged>(renderContainer_);

  // ----------------------
  // Init Domain Systems
  // ----------------------

  initGameWorld_();
  initChat_();

  // -----------------------
  // Init Networking
  // -----------------------

  gameNetwork_ = std::make_unique<GameNetwork>(
      appOptions_.url, chatRenderer_, gameWorldRenderer_, gameWorld_, gameTimer_);
  gameNetwork_->start();

  // -----------------------
  // Init Timers
  // -----------------------

  initTimers_();

  return SDL_APP_CONTINUE;
}

SDL_AppResult GameApp::onEvent(SDL_Event* event) {
  sdlImGuiWrapper_->onEvent(event);
  return gameInputManager_->applyEvent(event);
}

SDL_AppResult GameApp::iterate() {
  const float elapsed = sdlImGuiWrapper_->calculateDeltaTimeWhenFrameBegins();

  gameNetwork_->iterate();

  gameTimer_->iterate(elapsed);

  updateGameWorld_(elapsed);

  sdlImGuiWrapper_->render([this]() {
    renderContainer_->render();
  });

  gameInputManager_->onFrameEnd();

  PROFILER_FRAME_MARK;  // Mark end of frame for TRACY
  return SDL_APP_CONTINUE;
}

void GameApp::onQuit() {
  gameInputManager_->onAppQuit();
  sdlImGuiWrapper_->onQuit();
  gameNetwork_->stop();
}

// ------------
// Init Steps
// ------------

void GameApp::initTracyProfiler_() {
#if PROFILER_ENABLED
  SPDLOG_CRITICAL("PROFILER ENABLED");
#else
  SPDLOG_CRITICAL("PROFILER DISABLED");
#endif

  PROFILER_SET_THREAD_NAME("main");
}

void GameApp::initOptions_(int argc, char* argv[]) {
  try {
    // Example of usage:
    // GameEngine.exe --url wss://marleeeeeey.duckdns.org
    // GameEngine.exe --url ws://localhost:8083
    // GameEngine.exe --url wss://echo.websocket.org

    cxxopts::Options options(argv[0]);
    options.add_options()("u,url", "Url", cxxopts::value<std::string>()->default_value("wss://marleeeeeey.duckdns.org"));
    auto result = options.parse(argc, argv);
    appOptions_.url = result["url"].as<std::string>();
  } catch (const std::exception& e) {
    SPDLOG_CRITICAL("Command line parse error: {}", e.what());
  }
}

void GameApp::initGameWorld_() {
  gameWorldRenderer_ = IGameWorldRenderer::create(sdlImGuiWrapper_->getRenderer());
  gameWorld_ = std::make_shared<GameWorld>(INITIAL_WINDOWS_WIDTH, INITIAL_WINDOWS_HEIGHT, gameWorldRenderer_);

  gameWorld_->onPlayerPositionChanged = [this]() {
    Player& player = gameWorldRenderer_->myPlayer;
    gameNetwork_->sendPlayer(player);
  };

  renderContainer_->addComponent(gameWorldRenderer_);
  gameInputManager_->onWindowSizeChangedSink().connect<&GameWorld::onWindowSizeChanged>(gameWorld_);
}

void GameApp::initChat_() {
  chatRenderer_ = IChatRenderer::create();

  chatRenderer_->onMessageSentCallback = [this](const std::string& message) {
    ChatMessage chatMessage{
        // .sender - use default value. We don't need to send sender info. It will be updated on server.
        .message = message,
        .sentTimestamp = std::chrono::system_clock::now(),
    };

    gameNetwork_->sendChatMessage(chatMessage);
  };

  renderContainer_->addComponent(chatRenderer_);
}

void GameApp::initTimers_() {
  gameTimer_ = std::make_shared<GameTimer>();
  gameTimer_->scheduleRepeating(3, 3, -1, [this]() {
    gameNetwork_->sendPingFromClient();
  });
}

// ---------------------
// Basic Iterate Steps
// ---------------------

void GameApp::updateGameWorld_(const float elapsed) {
  PROFILER_ZONE;
  gameWorld_->iterate(elapsed, gameInputManager_->getGameInputData());
}
