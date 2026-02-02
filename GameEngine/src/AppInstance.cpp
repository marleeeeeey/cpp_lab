#include "AppInstance.h"

#define DEBUG_LOG_DISABLE_DEBUG_LEVEL
#include <DebugLog/DebugLog.h>
#include <NetworkManager/NetworkManagerFactory.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <sstream>

#include "ChatDataForRendering.h"
#include "GlobalConstants.h"
#include "cxxopts.hpp"

SDL_AppResult AppInstance::init(int argc, char* argv[]) {
  initOptions(argc, argv);
  auto initSdlResult = initSDL();  // initialize SDL and set renderer
  initImGui();
  sceneRenderer_.setRenderer(renderer_);
  gameWorld_.init();

  networkManager_ = NetworkManagerFactory::createNetworkManager();
  networkManager_->start(appOptions_.url);

  lastTime_ = SDL_GetTicks();
  return initSdlResult;
}

SDL_AppResult AppInstance::onEvent(SDL_Event* event) {
  ImGui_ImplSDL3_ProcessEvent(event);
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  userInputManger_.applyEvent(event);

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult AppInstance::iterate() {
  // -----------------------
  // Calculate Delta Time
  // -----------------------
  const Uint64 now = SDL_GetTicks();
  const float elapsed = ((float)(now - lastTime_)) / 1000.0f; /* seconds since last iteration */
  lastTime_ = now;

  // ----------------------------------
  // Poll network events (non-blocking)
  // ----------------------------------
  NetEvent ev{};
  int processed = 0;
  constexpr int kMaxEventsPerFrame = 256;
  while (processed < kMaxEventsPerFrame && networkManager_->poll(ev)) {
    ++processed;
    switch (ev.type) {
      case NetEvent::Type::Connected: {
        std::ostringstream ss;
        ss << "Connected to server " << appOptions_.url;
        debugLog() << "Net: " << ss.str() << std::endl;
        chatDataForRendering_.isConnected = true;
        chatDataForRendering_.addMessage(ss.str());
        break;
      }
      case NetEvent::Type::Disconnected: {
        debugLog() << "Net: Disconnected, reason=" << ev.payload << std::endl;
        chatDataForRendering_.isConnected = false;
        chatDataForRendering_.addMessage("Disconnected from server. Reason: " + ev.payload);
        networkManager_->start(appOptions_.url);  // try to reconnect
        break;
      }
      case NetEvent::Type::Error: {
        debugLog() << "Net: Error=" << ev.payload << std::endl;
        chatDataForRendering_.isConnected = false;
        chatDataForRendering_.addMessage("Error: " + ev.payload);
        networkManager_->start(appOptions_.url);  // try to reconnect
        break;
      }
      case NetEvent::Type::TextMessage: {
        debugLog() << "Net: Text=" << ev.payload << std::endl;
        chatDataForRendering_.addMessage(ev.payload);
        break;
      }
    }
  }

  // ---------------------------------------
  // Sent message to network every X seconds
  // ---------------------------------------
  gameTimeSeconds_ += elapsed;
  sendAccumSeconds_ += elapsed;
  constexpr double kSendPeriodSeconds = 5.0;
  if (sendAccumSeconds_ >= kSendPeriodSeconds) {
    sendAccumSeconds_ -= kSendPeriodSeconds;
    networkManager_->send(std::format("GameTime: {:.2f}", gameTimeSeconds_));
  }

  // -----------------------
  // Update game world
  // -----------------------
  gameWorld_.iterate(elapsed, userInputManger_.getUserInputData());
  GameDataForRendering gameDataForRendering = gameWorld_.getGameDataForRendering();

  // -----------------------
  // Clear Screen
  // -----------------------
  /* as you can see from this, rendering draws over whatever was drawn before it. */
  ImGuiIO& io = ImGui::GetIO();
  SDL_SetRenderScale(renderer_, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE); /* black, full alpha */
  SDL_RenderClear(renderer_);                                   /* start with a blank canvas. */

  // -----------------------
  // Begin Frame
  // -----------------------
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // -----------------------
  // Render New Frame
  // -----------------------
  sceneRenderer_.renderGameObjects(gameDataForRendering);
  // sceneRenderer.renderHelloWorldWindow();
  sceneRenderer_.renderChatWindow(chatDataForRendering_, [this](const std::string& message) {
    networkManager_->send(message);
  });
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);  // render the GUI
  SDL_RenderPresent(renderer_);                                            // show the rendered frame on screen

  userInputManger_.onFrameEnd();

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

void AppInstance::onQuit() {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  networkManager_->stop();
}

SDL_AppResult AppInstance::initSDL() {
  SDL_SetAppMetadata("Example Renderer Points", "1.0", "com.example.renderer-points");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("examples/renderer/points",
                                   WINDOW_WIDTH, WINDOW_HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &window_, &renderer_)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
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

void AppInstance::initImGui() {
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
  ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_);
  ImGui_ImplSDLRenderer3_Init(renderer_);
}

void AppInstance::initOptions(int argc, char* argv[]) {
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
    std::cerr << "Command line parse error: " << e.what() << std::endl;
  }
}
