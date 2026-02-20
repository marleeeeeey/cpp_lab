#include "GameApp.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <spdlog/spdlog.h>

#include <cxxopts.hpp>

#include "GameSharedObjects/ChatMessage.h"
#include "Profiler/Profiler.h"

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

  // TODO: move SDL and IMGui creation to separate class and use smart pointers

  auto initSdlResult = initSDL_();  // initialize SDL and set renderer
  if (initSdlResult != SDL_APP_CONTINUE) {
    return initSdlResult;
  }
  initImGui_();
  renderContainer_ = IRenderContainer::create(sdlRenderer_);
  gameInputManager_ = IGameInputManager::create(window_, INITIAL_WINDOWS_WIDTH, INITIAL_WINDOWS_HEIGHT);
  gameInputManager_->onWindowSizeChangedSink().connect<&IRenderContainer::onWindowSizeChanged>(renderContainer_);

  // ----------------------
  // Init Domain Systems
  // ----------------------

  initGameWorld_();
  initChat_();

  // -----------------------
  // Init Networking
  // -----------------------

  gameNetwork_ = std::make_unique<GameNetwork>(appOptions_.url, chatRenderer_, gameWorldRenderer_, gameWorld_);
  gameNetwork_->start();

  beginFrameTime_ = SDL_GetTicks();

  return SDL_APP_CONTINUE;
}

SDL_AppResult GameApp::onEvent(SDL_Event* event) {
  ImGui_ImplSDL3_ProcessEvent(event);
  return gameInputManager_->applyEvent(event);
}

SDL_AppResult GameApp::iterate() {
  const float elapsed = calculateDeltaTime_();

  gameNetwork_->iterate();

  updateGameWorld_(elapsed);

  renderFrame_();

  gameInputManager_->onFrameEnd();

  PROFILER_FRAME_MARK;  // Mark end of frame for TRACY

  return SDL_APP_CONTINUE;
}

void GameApp::onQuit() {
  gameInputManager_->onAppQuit();

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

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

SDL_AppResult GameApp::initSDL_() {
  SDL_SetAppMetadata("Example Renderer Points", "1.0", "com.example.renderer-points");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SPDLOG_ERROR("SDL_Init() failed: {}", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("GameEngine", INITIAL_WINDOWS_WIDTH, INITIAL_WINDOWS_HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &window_, &sdlRenderer_)) {
    SPDLOG_ERROR("SDL_CreateWindowAndRenderer() failed: {}", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // VSync should be enabled to decrease CPU loading!
  if (!SDL_SetRenderVSync(sdlRenderer_, 1)) {
    SPDLOG_ERROR("SDL_SetRenderVSync() failed: {}", SDL_GetError());
  }

  // IMPORTANT:
  // SDL logical presentation (LETTERBOX) changes render coordinates,
  // but mouse events remain in window coords,
  // which breaks ImGui hit-testing after resize.
  // Keep renderer in native window coordinates when using ImGui.
  // The next line should be commented.
  // SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

void GameApp::initImGui_() {
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

  // ------------------------
  // Setup Dear ImGui context
  // ------------------------
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // ------------------------
  // Setup Dear ImGui style
  // ------------------------
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // ------------------------
  // Setup scaling
  // ------------------------
  ImGuiStyle& style = ImGui::GetStyle();
  // Bake a fixed style scale. (until we have a solution for dynamic style scaling,
  // changing this requires resetting Style + calling this again)
  style.ScaleAllSizes(main_scale);
  // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary.
  // We leave both here for documentation purpose)
  style.FontScaleDpi = main_scale;

  // ---------------------------------
  // Setup Platform/Renderer backends
  // ---------------------------------
  ImGui_ImplSDL3_InitForSDLRenderer(window_, sdlRenderer_);
  ImGui_ImplSDLRenderer3_Init(sdlRenderer_);

  // ---------------------------------
  // Load Fonts
  // ---------------------------------

  ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf");
  if (font == nullptr) {
    SPDLOG_CRITICAL("Failed to load font");
  }
}

void GameApp::initGameWorld_() {
  gameWorldRenderer_ = IGameWorldRenderer::create(sdlRenderer_);
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

// ---------------------
// Basic Iterate Steps
// ---------------------

float GameApp::calculateDeltaTime_() {
  PROFILER_ZONE;

  const Uint64 now = SDL_GetTicks();
  /* seconds since last iteration */
  const float elapsed = ((float)(now - beginFrameTime_)) / 1000.0f;
  beginFrameTime_ = now;
  return elapsed;
}

void GameApp::updateGameWorld_(const float elapsed) {
  PROFILER_ZONE;
  gameWorld_->iterate(elapsed, gameInputManager_->getGameInputData());
}

void GameApp::renderFrame_() {
  {
    PROFILER_ZONE_NAMED("RenderFrame");

    // --------------------------------
    // Clear Screen (fill with black)
    // --------------------------------
    ImGuiIO& io = ImGui::GetIO();
    SDL_SetRenderScale(sdlRenderer_, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(sdlRenderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(sdlRenderer_);

    // -----------------------
    // Begin Frame
    // -----------------------
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // -----------------------
    // Render New Frame
    // -----------------------
    renderContainer_->render();
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer_);  // render the GUI
  }

  {
    PROFILER_ZONE_NAMED("SDL_RenderPresent (vsync wait)");
    SDL_RenderPresent(sdlRenderer_);  // show the rendered frame on screen
  }
}
