#include <NetworkTransport/TransportFactory.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <future>

std::shared_ptr<ITransport> globalTransport;

int main(int argc, char** argv) {
  // -----------------------------------------------------------
  // Parsing arguments
  // WsConsoleClient.exe --url ws://your.service.com:8080
  // WsConsoleClient.exe --url ws://127.0.0.1:9001
  // -----------------------------------------------------------
  std::string url = "wss://echo.websocket.org";
  for (int i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--url" && i + 1 < argc) {
      url = argv[i + 1];
    }
  }

  // ---------------------------------------
  // Get transport interface
  // ---------------------------------------
  globalTransport = createTransport();
  std::weak_ptr<ITransport> weak = globalTransport;

  // ---------------------------------------
  // Initiate connection and message loop
  // ---------------------------------------
  globalTransport->onOpen = []() {
    SPDLOG_INFO("Connected to server");
  };
  globalTransport->onError = [](std::string_view errorMsg) {
    SPDLOG_ERROR("Network error: {}", errorMsg);
  };
  globalTransport->onText = [weak](std::string_view msg) {
    if (auto transport = weak.lock()) {
      SPDLOG_INFO("Recv: {}", msg);
      std::string initialMsg = "Hello from server!";
      transport->sendText(initialMsg);
      SPDLOG_INFO("Sent: {}", initialMsg);
    }
  };
  globalTransport->connect(url);

#ifdef __EMSCRIPTEN__
  // There is no possible to pause the program in a browser - this case browser will freeze.
  // For this tiny example allow a transport object to live.
  // Because the browser still works with this socket.
#else
  SPDLOG_INFO("Press any key to exit");
  std::getchar();

  // ---------------------------------------
  // Closing connection
  // ---------------------------------------
  globalTransport.reset();
  SPDLOG_INFO("Network transport destroyed");
  return 0;
#endif

  SPDLOG_INFO("main() finished");
}