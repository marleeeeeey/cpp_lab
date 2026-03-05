#include "ClientApp.h"

#include <GameUtils/GeomUtils.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include <cxxopts.hpp>

#include "ClientWorldSimulation.h"
#include "GameShared/ChatMessage.h"
#include "GameShared/SimulationTickRate.h"
#include "Profiler/Profiler.h"

// ---------------------------
// ISdlApp Factory Method
// ---------------------------

std::unique_ptr<ISdlApp> ISdlApp::create() {
  return std::make_unique<ClientApp>();
}

// ------------------
// SDL Based Steps
// ------------------

ClientApp::~ClientApp() {
  sdlWrapper_->onQuit();
  gameNetwork_->stop();
  SPDLOG_CRITICAL("ClientApp destroyed");
}

SDL_AppResult ClientApp::init(int argc, char* argv[]) {
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
  dispatcher_.sink<KeyPressed>().connect<&ClientApp::onKeyPressed>(this);

  // ----------------------
  // Init Domain Systems
  // ----------------------

  debugRender_ = IDebugRender::create();

  initGameWorld_();
  initChat_();

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

  auto onWorldSnapshotReceivedCb = [this](const WorldSnapshot& snapshot) {
    clientWorldSimulation_->addSnapshot(snapshot);
  };

  gameNetwork_ = std::make_unique<ClientNetwork>(
      appOptions_.url,
      debugRender_,
      chatRenderer_,
      gameWorldRenderer_,
      snowflakesSimulation_,
      gameTimer_,
      clientWorldSimulation_,
      onWorldSnapshotReceivedCb);
  gameNetwork_->start();

  // -----------------------
  // Init Timers
  // -----------------------

  initTimers_();

  return SDL_APP_CONTINUE;
}

SDL_AppResult ClientApp::onEvent(SDL_Event* event) {
  sdlWrapper_->onEvent(event);
  return userInputManager_->applyEvent(event);
}

void ClientApp::sendUserInputToServer_() {
  auto userInputData = userInputManager_->getUserInputData();

  if (!userInputData.keyboardInputChanged) {
    return;
  }

  // ---------------------------
  // Calculate input direction
  // ---------------------------

  glm::vec2 dir(0.0f, 0.0f);
  if (userInputData.keyboard.held.up) dir.y -= 1.0f;
  if (userInputData.keyboard.held.down) dir.y += 1.0f;
  if (userInputData.keyboard.held.left) dir.x -= 1.0f;
  if (userInputData.keyboard.held.right) dir.x += 1.0f;
  dir = GeomUtils::safeNormalize(dir);

  // ---------------------------
  // Send input direction
  // ---------------------------

  InputPacket inputPacket;
  inputPacket.moveX = dir.x;
  inputPacket.moveY = dir.y;
  SPDLOG_TRACE("Sending input: ({}, {})", inputPacket.moveX, inputPacket.moveY);
  gameNetwork_->sendInputPacketFromClient(inputPacket);
  clientWorldSimulation_->setMyLastInputPacket(inputPacket);
}

// https://gafferongames.com/post/fix_your_timestep/
SDL_AppResult ClientApp::iterate() {
  float frameTimeSeconds = sdlWrapper_->calculateDeltaTimeWhenFrameBegins();
  constexpr float dt = 1.0f / CLIENT_SIMULATION_TICK_RATE;
  constexpr float maxFrameTimeSeconds = 10 * dt;  // 10 times over dt

  // --------------------------------------------------------------
  // Limit frame time in case of an intense load of simulation
  // --------------------------------------------------------------

  if (frameTimeSeconds > maxFrameTimeSeconds) {
    frameTimeSeconds = maxFrameTimeSeconds;
  }

  // -------------------------------
  // Non blocking iterate methods
  // -------------------------------

  gameNetwork_->iterate();
  sendUserInputToServer_();

  // ----------------------------
  // Fixed rate simulation
  // ----------------------------

  accumulator_ += frameTimeSeconds;

  while (accumulator_ >= dt) {
    // prevState = curState // TODO
    gameTimer_->iterate(dt);
    float gameTime = gameTimer_->time();
    iterateGameWorld_(dt, gameTime);
    accumulator_ -= dt;
  }

  // ---------------------------
  // Interpolate via blending // TODO
  // ---------------------------

  // const double alpha = accumulator / dt;
  // State state = currentState * alpha +
  //     previousState * ( 1.0 - alpha );

  // ------------
  // Rendering
  // ------------

  iterateDebugRender_();
  sdlWrapper_->render([this]() {
    renderContainer_->render();
  });

  // ----------------
  // Finalization
  // ----------------

  userInputManager_->onFrameEnd();
  PROFILER_FRAME_MARK;  // Mark end of frame for TRACY
  return SDL_APP_CONTINUE;
}

// -------------------
// Input and Events
// -------------------

void ClientApp::onKeyPressed(const KeyPressed& keyPressed) {
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

void ClientApp::initTracyProfiler_() {
#if PROFILER_ENABLED
  SPDLOG_CRITICAL("PROFILER ENABLED");
#else
  SPDLOG_CRITICAL("PROFILER DISABLED");
#endif

  PROFILER_SET_THREAD_NAME("main");
}

void ClientApp::initOptions_(int argc, char* argv[]) {
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

void ClientApp::initGameWorld_() {
  gameWorldRenderer_ = IGameWorldRenderer::create(sdlWrapper_->getRenderer());
  clientWorldSimulation_ = std::make_shared<ClientWorldSimulation>(debugRender_);
  snowflakesSimulation_ = std::make_shared<SnowflakesSimulation>(gameWorldRenderer_);
  sdlWrapper_->onWindowSizeChangedSink().connect<&SnowflakesSimulation::onWindowSizeChanged>(snowflakesSimulation_);
}

void ClientApp::initChat_() {
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

void ClientApp::initTimers_() {
  gameTimer_ = std::make_shared<GameTimer>();
  gameTimer_->scheduleRepeating(3, 3, -1, [this]() {
    gameNetwork_->sendPingFromClient();
  });
}

// ---------------------
// Basic Iterate Steps
// ---------------------

void ClientApp::iterateGameWorld_(const float dt, const float gameTime) {
  PROFILER_ZONE;
  snowflakesSimulation_->iterate(dt);
  clientWorldSimulation_->iterate(dt, gameTime);
  gameWorldRenderer_->setWorldSnapshot(clientWorldSimulation_->getResultSnapshot());
}

void ClientApp::iterateDebugRender_() {
  if (!debugRender_) return;

  auto& userInputData = userInputManager_->getUserInputData();
  auto& m = userInputData.mouse;

  constexpr bool showTouchDebug = false;  // TODO: enable when create mobile touch/mouse support
  if (showTouchDebug) {
    debugRender_->addLine(std::format("Mouse diff: ({}, {})", m.dx, m.dy));
    debugRender_->addLine(std::format("Mouse abs: ({}, {})", m.winX, m.winY));
    debugRender_->addLine(std::format("Mouse screen: ({}, {})", m.screenX, m.screenY));
    debugRender_->addLine(std::format("Mouse wheel: ({}, {})", m.wheelX, m.wheelY));
  }
}
