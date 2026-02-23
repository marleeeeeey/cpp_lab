#include "SdlApp/SdlWithImGuiWrapper.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

#include "Profiler/Profiler.h"

constexpr int INITIAL_WINDOWS_WIDTH = 800;
constexpr int INITIAL_WINDOWS_HEIGHT = 600;

// -----------------------
// Public Interface
// -----------------------

SdlWithImGuiWrapper::SdlWithImGuiWrapper() {
  auto initSdlResult = initSDL_();  // initialize SDL and set renderer
  if (initSdlResult != SDL_APP_CONTINUE) {
    SPDLOG_CRITICAL("SDL_IMGUI_Wrapper failed to initialize SDL: Code {}. {}", (int)initSdlResult, SDL_GetError());
    throw std::runtime_error("SDL_IMGUI_Wrapper failed to initialize SDL");
  }
  initImGui_();

  beginFrameTime_ = SDL_GetTicks();
}

bool SdlWithImGuiWrapper::onEvent(SDL_Event* event) {
  bool isInputHandled = ImGui_ImplSDL3_ProcessEvent(event);

  if (event->type == SDL_EVENT_WINDOW_RESIZED) {
    SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(windowWidth_), static_cast<float>(windowHeight_));
    onWindowSizeChangedSignal_.publish(windowWidth_, windowHeight_);
  }

  return isInputHandled;
}

float SdlWithImGuiWrapper::calculateDeltaTimeWhenFrameBegins() {
  PROFILER_ZONE;

  // send initial window size event
  if (!onWindowSizeChangedInitialCallbackCalled_) {
    onWindowSizeChangedInitialCallbackCalled_ = true;
    SDL_GetWindowSize(window_, &windowWidth_, &windowHeight_);
    onWindowSizeChangedSignal_.publish(windowWidth_, windowHeight_);
  }

  const Uint64 now = SDL_GetTicks();
  const float elapsed = static_cast<float>(now - beginFrameTime_) / 1000.0f;
  beginFrameTime_ = now;
  return elapsed;
}

void SdlWithImGuiWrapper::render(std::function<void()> userRenderCallback) {
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
    // Prepare ImGui Layout
    // -----------------------

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGui::DockSpaceOverViewport(0, 0, ImGuiDockNodeFlags_PassthruCentralNode);
    }

    // -------------------------
    // Prepare User Rendering
    // -------------------------

    if (userRenderCallback) {
      userRenderCallback();
    }

    // ----------------------
    // Render Everything
    // ----------------------

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), sdlRenderer_);
  }

  {
    PROFILER_ZONE_NAMED("SDL_RenderPresent (vsync wait)");
    SDL_RenderPresent(sdlRenderer_);  // show the rendered frame on screen
  }
}

void SdlWithImGuiWrapper::onQuit() {
  onWindowSizeChangedSink().disconnect();

  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}

entt::sink<SdlWithImGuiWrapper::OnWindowSizeChangedSignal> SdlWithImGuiWrapper::onWindowSizeChangedSink() {
  return entt::sink{onWindowSizeChangedSignal_};
}

void SdlWithImGuiWrapper::confineMouseCursorToWindow(bool flag) {
  // TODO: Continue receiving events outside window - DOES NOT WORK
  // SDL_CaptureMouse(true);

  SDL_SetWindowMouseGrab(window_, flag);
}

// -----------------------
// Private Methods
// -----------------------

SDL_AppResult SdlWithImGuiWrapper::initSDL_() {
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

  return SDL_APP_CONTINUE; /* carry on with the program! */
}

void SdlWithImGuiWrapper::initImGui_() {
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

  // -------------
  // Load Fonts
  // -------------

  ImFont* font = io.Fonts->AddFontFromFileTTF("assets/fonts/OpenSans-VariableFont_wdth,wght.ttf");
  if (font == nullptr) {
    SPDLOG_CRITICAL("Failed to load font");
  }

  // --------------------------
  // Enable windows docking
  // --------------------------

  // TODO: Here is a known issue with current ImGui with Docking Mode:
  // Background opacity always ignored when docking to central node.
  // It hide SDL_renderer content with game objects.
  // ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}
