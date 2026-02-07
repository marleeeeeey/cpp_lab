#pragma once
#include <functional>
#include <string>
#include <string_view>

// ---------------------
// Forward Declarations
// ---------------------

struct SDL_Renderer;
struct GameDataForRendering;
class ChatDataForRendering;

// ----------------------
// SceneRenderer class
// ----------------------

// Renders the game world and ImGui GUI. Called by AppInstance.
class SceneRenderer {
  SDL_Renderer* renderer_ = nullptr;

 public:
  void setRenderer(SDL_Renderer* renderer);

  void onWindowSizeChanged(int width, int height);

  void renderGameObjects(const GameDataForRendering& gameDataForRendering);
  using OnMessageSentCallback = std::function<void(const std::string&)>;
  void renderChatWindow(const ChatDataForRendering& chatDataForRendering,
                        const OnMessageSentCallback& onMessageSentCallback);
};
