#include "AppInstance.h"

#include <debug_log/DebugLog.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <transport_layer/TransportFactory.h>

#include <sstream>

#include "GlobalConstants.h"

SDL_AppResult AppInstance::init() {
  auto initSdlResult = initSDL();  // initialize SDL and set renderer
  initImGui();
  sceneRenderer.setRenderer(renderer);
  gameWorld.init();
  initNetwork();
  last_time = SDL_GetTicks();
  return initSdlResult;
}

SDL_AppResult AppInstance::onEvent(SDL_Event* event) {
  ImGui_ImplSDL3_ProcessEvent(event);
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }

  userInputManger.applyEvent(event);

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

SDL_AppResult AppInstance::iterate() {
  // -----------------------
  // Calculate Delta Time
  // -----------------------
  const Uint64 now = SDL_GetTicks();
  const float elapsed = ((float)(now - last_time)) / 1000.0f; /* seconds since last iteration */
  last_time = now;

  // ---------------------------------------
  // Sent message to network every 5 seconds
  // ---------------------------------------
  gameTimeSeconds += elapsed;
  sendAccumSeconds += elapsed;
  constexpr double kSendPeriodSeconds = 5.0;
  if (sendAccumSeconds >= kSendPeriodSeconds) {
    sendAccumSeconds -= kSendPeriodSeconds;
    networkTransport->sendText(std::format("GameTime: {:.2f}", gameTimeSeconds));
  }

  // -----------------------
  // Update game world
  // -----------------------
  gameWorld.iterate(elapsed, userInputManger.getUserInputData());
  GameDataForRendering gameDataForRendering = gameWorld.getGameDataForRendering();

  // -----------------------
  // Clear Screen
  // -----------------------
  /* as you can see from this, rendering draws over whatever was drawn before it. */
  ImGuiIO& io = ImGui::GetIO();
  SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); /* black, full alpha */
  SDL_RenderClear(renderer);                                   /* start with a blank canvas. */

  // -----------------------
  // Begin Frame
  // -----------------------
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  // -----------------------
  // Render New Frame
  // -----------------------
  sceneRenderer.render(gameDataForRendering);
  ImGui::Render();
  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);  // render the GUI
  SDL_RenderPresent(renderer);                                            // show the rendered frame on screen

  userInputManger.onFrameEnd();

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

void AppInstance::onQuit() {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  networkTransport->close();
}

SDL_AppResult AppInstance::initSDL() {
  SDL_SetAppMetadata("Example Renderer Points", "1.0", "com.example.renderer-points");

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!SDL_CreateWindowAndRenderer("examples/renderer/points",
                                   WINDOW_WIDTH, WINDOW_HEIGHT,
                                   SDL_WINDOW_RESIZABLE, &window, &renderer)) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);
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
  style.ScaleAllSizes(main_scale);  // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
  style.FontScaleDpi = main_scale;  // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

  // ---------------------------------
  // Setup Platform/Renderer backends
  // ---------------------------------
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);
}

void AppInstance::initNetwork() {
  std::string url = "wss://echo.websocket.org";

  networkTransport = createTransport();

  // ---------------------------------------
  // Initiate connection and message loop
  // ---------------------------------------
  networkTransport->onOpen = []() {
    debugLog() << "Connected to server" << std::endl;
  };
  networkTransport->onError = [](std::string_view errorMsg) {
    std::cerr << "Failed to connect: " << errorMsg << std::endl;
  };
  networkTransport->onText = [](std::string_view msg) {
    debugLog() << "Recv: " << msg << std::endl;
  };
  networkTransport->onClose = [](int code, std::string_view reason) {
    debugLog() << "Connection closed. Code: " << code << ", reason: " << reason << std::endl;
  };
  networkTransport->connect(url);
}
