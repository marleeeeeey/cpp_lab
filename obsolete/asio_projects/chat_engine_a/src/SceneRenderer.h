#pragma once
#include <functional>
#include <string>

struct SDL_Renderer;
struct DataForRendering;

// Renders GUI for chat.
class SceneRenderer {
 public:
  void setRenderer(SDL_Renderer* renderer);
  void renderGUI(const DataForRendering& dataForRendering);

  using OnMessageSentCallback = std::function<void(const std::string&)>;
  void setOnMessageSent(const OnMessageSentCallback& callback);

 private:
  SDL_Renderer* renderer = nullptr;
  OnMessageSentCallback onMessageSent;
};
