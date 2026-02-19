#include "GameApp.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <spdlog/spdlog.h>

#include <cxxopts.hpp>

#include "GameMessageTypes/GameMessageTypes.h"
#include "GameSharedObjects/ChatMessage.h"
#include "Profiler/Profiler.h"
#include "SerializationProtocol/SerializationProtocol.h"
#include "magic_enum/magic_enum.hpp"

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

  // ----------
  // Init SDL
  // ----------

  auto initSdlResult = initSDL_();  // initialize SDL and set renderer
  if (initSdlResult != SDL_APP_CONTINUE) {
    return initSdlResult;
  }

  // ----------------------
  // Init Main Systems
  // ----------------------

  initImGui_();
  initRenderContainer_();
  initGameWorld_();
  initChat_();
  initNetworkHandlers_();

  beginFrameTime_ = SDL_GetTicks();

  return SDL_APP_CONTINUE;
}

SDL_AppResult GameApp::onEvent(SDL_Event* event) {
  ImGui_ImplSDL3_ProcessEvent(event);

  if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);
    onWindowSizeChangedSignal_.publish(windowWidth_, windowHeight_);
  }

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  userInputManger_.applyEvent(event);

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult GameApp::iterate() {
  const float elapsed = calculateDeltaTime_();

  autoReconnectionNetwork_->iterate();

  updateGameWorld_(elapsed);

  renderFrame_();

  userInputManger_.onFrameEnd();

  PROFILER_FRAME_MARK;  // Mark end of frame for TRACY

  return SDL_APP_CONTINUE;
}

void GameApp::onQuit() {
  onWindowSizeChangedSink().disconnect();

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  autoReconnectionNetwork_->stop();
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

  if (!SDL_CreateWindowAndRenderer("GameEngine", windowWidth_, windowHeight_,
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

void GameApp::initRenderContainer_() {
  renderContainer_ = IRenderContainer::create(sdlRenderer_);
  onWindowSizeChangedSink().connect<&IRenderContainer::onWindowSizeChanged>(renderContainer_);
}

void GameApp::initGameWorld_() {
  gameWorldRenderer_ = IGameWorldRenderer::create(sdlRenderer_);
  gameWorld_.init(windowWidth_, windowHeight_, gameWorldRenderer_);
  renderContainer_->addRenderer(gameWorldRenderer_);
  onWindowSizeChangedSink().connect<&GameWorld::onWindowSizeChanged>(gameWorld_);
}

void GameApp::initChat_() {
  chatRenderer_ = IChatRenderer::create();

  chatRenderer_->onMessageSentCallback = [this](const std::string& message) {
    ChatMessage chatMessage{
        // .sender - use default value. We don't need to send sender info. It will be updated on server.
        .message = message,
        .sentTimestamp = std::chrono::system_clock::now(),
    };

    // IMPROVE: do both operations in one call and one memory allocation
    auto payload = SerializationProtocol::serializeChatMessage(chatMessage);
    payload = networkDataHandler_->addTypeForBinaryMessage(GMT_ChatMessage, payload);

    autoReconnectionNetwork_->sendBinary(payload);
  };

  renderContainer_->addRenderer(chatRenderer_);
}

void GameApp::initNetworkHandlers_() {
  networkDataHandler_ = INetworkDataHandler::create();

  networkDataHandler_->registerCallbackForTextMessages(
      [this](std::string_view textMessage) {
        ChatMessage chatMessage{
            .sender = Player{
                .name = "Anonymous",
                .messagesSent = 0,
            },
            .message = std::string(textMessage),
        };
        if (chatRenderer_) {
          chatRenderer_->addMessage(chatMessage);
        }
        SPDLOG_INFO("Text Message received: {}", chatMessage.message);
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_ChatMessage,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        auto newChatMessage = SerializationProtocol::deserializeChatMessage(payload);
        newChatMessage.receivedTimestamp = std::chrono::system_clock::now();
        if (chatRenderer_) {
          chatRenderer_->addMessage(newChatMessage);
        }
        SPDLOG_TRACE("Message type {} received: {}", type, newChatMessage.message);
      });

  networkDataHandler_->registerCallbackForBinaryMessageWithType(
      GMT_NumberOfClients,
      [this](const auto type, const std::vector<uint8_t>& payload) {
        // unpack number of users
        int receivedNumber = 0;
        std::memcpy(&receivedNumber, payload.data(), sizeof(receivedNumber));

        // set and log
        if (chatRenderer_) {
          chatRenderer_->numberOfConnectedUsers = receivedNumber;
        }
        SPDLOG_TRACE("Message type {} received: {}", type, receivedNumber);
      });

  autoReconnectionNetwork_ = IAutoReconnectionNetwork::create();
  autoReconnectionNetwork_->init(
      appOptions_.url,
      [this](std::string_view textMessage) {
        SPDLOG_TRACE("Received text message: {}", textMessage);
        networkDataHandler_->notifyAboutTextMessage(textMessage);
      },
      [this](std::vector<uint8_t> binaryMessage) {
        SPDLOG_TRACE("Received binary message");
        networkDataHandler_->notifyAboutBinaryMessage(binaryMessage);
      },
      [this](IAutoReconnectionNetwork::State newState) {
        if (chatRenderer_) {
          chatRenderer_->connectionStatus = magic_enum::enum_name(newState);
        }
      });
  autoReconnectionNetwork_->start();
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
  gameWorld_.iterate(elapsed, userInputManger_.getUserInputData());
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
