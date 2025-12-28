#include "AppInstance.h"

#include <imgui/imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <sstream>

#include "GlobalConstants.h"

SDL_AppResult AppInstance::init() {
  auto initSdlResult = initSDL();  // initialize SDL and set renderer
  initImGui();
  sceneRenderer.setRenderer(renderer);
  gameWorld.init();
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
  //
  // Calculate Delta Time
  //
  const Uint64 now = SDL_GetTicks();
  const float elapsed = ((float)(now - last_time)) / 1000.0f; /* seconds since last iteration */
  last_time = now;

  //
  // Update game world
  //
  gameWorld.iterate(elapsed, userInputManger.getUserInputData());
  GameDataForRendering gameDataForRendering = gameWorld.getGameDataForRendering();

  //
  // Clear Screen
  //
  /* as you can see from this, rendering draws over whatever was drawn before it. */
  ImGuiIO& io = ImGui::GetIO();
  SDL_SetRenderScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); /* black, full alpha */
  SDL_RenderClear(renderer);                                   /* start with a blank canvas. */

  //
  // Begin Frame
  //
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  //
  // Render New Frame
  //
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

  /* SDL will clean up the window/renderer for us. */
  // SDL_DestroyRenderer(renderer);
  // SDL_DestroyWindow(window);
  // SDL_Quit();
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

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle& style = ImGui::GetStyle();
  style.ScaleAllSizes(main_scale);  // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
  style.FontScaleDpi = main_scale;  // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer3_Init(renderer);

  // Load Fonts
  // - If fonts are not explicitly loaded, Dear ImGui will call AddFontDefault() to select an embedded font: either AddFontDefaultVector() or AddFontDefaultBitmap().
  //   This selection is based on (style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi) reaching a small threshold.
  // - You can load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
  // - If a file cannot be loaded, AddFont functions will return a nullptr. Please handle those errors in your code (e.g. use an assertion, display an error and quit).
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use FreeType for higher quality font rendering.
  // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
  // style.FontSizeBase = 20.0f;
  // io.Fonts->AddFontDefaultVector();
  // io.Fonts->AddFontDefaultBitmap();
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
  // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
  // IM_ASSERT(font != nullptr);
}
