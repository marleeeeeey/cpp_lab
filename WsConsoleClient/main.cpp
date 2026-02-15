#include <NetworkTransport/INetworkTransport.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cxxopts.hpp>
#include <future>

std::shared_ptr<INetworkTransport> globalTransport;

int main(int argc, char** argv) {
  // -------------------
  // Parsing arguments
  // -------------------

  // Examples:
  // WsConsoleClient.exe --url ws://your.service.com:8080
  // WsConsoleClient.exe --url ws://127.0.0.1:9001
  // WsConsoleClient.exe --url wss://echo.websocket.org
  std::string url;
  try {
    cxxopts::Options options(argv[0]);
    options.add_options()("u,url", "Url", cxxopts::value<std::string>()->default_value("wss://echo.websocket.org"));
    auto result = options.parse(argc, argv);
    url = result["url"].as<std::string>();
  } catch (const std::exception& e) {
    SPDLOG_CRITICAL("Command line parse error: {}", e.what());
    return 1;
  }

  // ---------------------------------------
  // Get transport interface
  // ---------------------------------------
  globalTransport = INetworkTransport::create();
  std::weak_ptr<INetworkTransport> weak = globalTransport;

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