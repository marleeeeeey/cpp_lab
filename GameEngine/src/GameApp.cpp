#include "GameApp.h"

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <cxxopts.hpp>

#include "GameShared/ChatMessage.h"
#include "Profiler/Profiler.h"

// ---------------------------
// ISdlApp Factory Method
// ---------------------------

std::unique_ptr<ISdlApp> ISdlApp::create() {
  return std::make_unique<GameApp>();
}

// ------------------
// SDL Based Steps
// ------------------

GameApp::~GameApp() {
  sdlWrapper_->onQuit();
  gameNetwork_->stop();
  SPDLOG_CRITICAL("GameApp destroyed");
}

SDL_AppResult GameApp::init(int argc, char* argv[]) {
  // ---------------------------
  // Init Auxiliary Components
  // ---------------------------

  // spdlog::set_level(spdlog::level::trace);
  initOptions_(argc, argv);
  initTracyProfiler_();

  // -----------------
  // Init SDL, IMGui
  // -----------------

  sdlWrapper_ = std::make_unique<SdlWithImGuiWrapper>();
  renderContainer_ = IRenderContainer::create(sdlWrapper_->getRenderer());

  // ---------------------
  // Init Input Manager
  // ---------------------

  userInputManager_ = IUserInputManager::create(dispatcher_);
  dispatcher_.sink<KeyPressed>().connect<&GameApp::onKeyPressed>(this);

  // ----------------------
  // Init Domain Systems
  // ----------------------

  initGameWorld_();
  initChat_();

  debugRender_ = IDebugRender::create();
  debugRender_->setVisible(showDebugWindows_);
  debugRender_->setOnDebugToggleCallback([this]() {
    showDebugWindows_ = !showDebugWindows_;
    debugRender_->setVisible(showDebugWindows_);
    chatRenderer_->setVisible(showDebugWindows_);
  });

  // -----------------------
  // Set Rendering Order
  // -----------------------

  renderContainer_->addComponent(chatRenderer_);
  renderContainer_->addComponent(debugRender_);
  renderContainer_->addComponent(gameWorldRenderer_);

  // -----------------------
  // Init Networking
  // -----------------------

  gameNetwork_ = std::make_unique<GameNetwork>(
      appOptions_.url,
      debugRender_,
      chatRenderer_,
      gameWorldRenderer_,
      gameWorld_,
      gameTimer_);
  gameNetwork_->start();

  // -----------------------
  // Init Timers
  // -----------------------

  initTimers_();

  return SDL_APP_CONTINUE;
}

SDL_AppResult GameApp::onEvent(SDL_Event* event) {
  sdlWrapper_->onEvent(event);
  return userInputManager_->applyEvent(event);
}

SDL_AppResult GameApp::iterate() {
  const float elapsed = sdlWrapper_->calculateDeltaTimeWhenFrameBegins();

  gameNetwork_->iterate();

  gameTimer_->iterate(elapsed);

  // TODO: move it out
  auto userInputData = userInputManager_->getUserInputData();
  glm::vec2 dir(0.0f, 0.0f);
  if (userInputData.keyboard.held.up) dir.y -= 1.0f;
  if (userInputData.keyboard.held.down) dir.y += 1.0f;
  if (userInputData.keyboard.held.left) dir.x -= 1.0f;
  if (userInputData.keyboard.held.right) dir.x += 1.0f;
  const bool hasInput = (dir.x != 0.0f || dir.y != 0.0f);
  dir = glm::normalize(dir);
  if (hasInput) {
    InputPacket inputPacket;
    inputPacket.moveX = dir.x;
    inputPacket.moveY = dir.y;
    SPDLOG_INFO("Sending input: ({}, {})", inputPacket.moveX, inputPacket.moveY);
    gameNetwork_->sendInputPacketFromClient(inputPacket);
  }

  iterateGameWorld_(elapsed);

  iterateDebugRender_();

  sdlWrapper_->render([this]() {
    renderContainer_->render();
  });

  userInputManager_->onFrameEnd();

  PROFILER_FRAME_MARK;  // Mark end of frame for TRACY
  return SDL_APP_CONTINUE;
}

// -------------------
// Input and Events
// -------------------

void GameApp::onKeyPressed(const KeyPressed& keyPressed) {
  // Tilda
  if (keyPressed.scancode == SDL_SCANCODE_GRAVE && !keyPressed.repeat) {
    showDebugWindows_ = !showDebugWindows_;
    debugRender_->setVisible(showDebugWindows_);
    chatRenderer_->setVisible(showDebugWindows_);
  }
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
  gameWorldRenderer_ = IGameWorldRenderer::create(sdlWrapper_->getRenderer());
  gameWorld_ = std::make_shared<GameWorld>(gameWorldRenderer_);
  sdlWrapper_->onWindowSizeChangedSink().connect<&GameWorld::onWindowSizeChanged>(gameWorld_);
}

void GameApp::initChat_() {
  chatRenderer_ = IChatRenderer::create();
  chatRenderer_->setVisible(showDebugWindows_);

  chatRenderer_->onMessageSentCallback = [this](const std::string& message) {
    ChatMessage chatMessage{
        // .sender - use default value. We don't need to send sender info. It will be updated on server.
        .message = message,
        .sentTimestamp = std::chrono::system_clock::now(),
    };

    gameNetwork_->sendChatMessage(chatMessage);
  };
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

void GameApp::iterateGameWorld_(const float elapsed) {
  PROFILER_ZONE;
  gameWorld_->iterate(elapsed, userInputManager_->getUserInputData());
}

void GameApp::iterateDebugRender_() {
  if (!debugRender_) return;

  auto& userInputData = userInputManager_->getUserInputData();
  auto& m = userInputData.mouse;

  debugRender_->addLine(std::format("Mouse diff: ({}, {})", m.dx, m.dy));
  debugRender_->addLine(std::format("Mouse abs: ({}, {})", m.winX, m.winY));
  debugRender_->addLine(std::format("Mouse screen: ({}, {})", m.screenX, m.screenY));
  debugRender_->addLine(std::format("Mouse wheel: ({}, {})", m.wheelX, m.wheelY));
}
