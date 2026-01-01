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

 public:
  using MessageSentCallback = std::function<void(const std::string&)>;
  void setOnMessageSent(const MessageSentCallback& callback);

 private:
  SDL_Renderer* renderer = nullptr;
  MessageSentCallback onMessageSent;
};
