#include <transport_layer/TransportFactory.h>

#include <chrono>
#include <future>
#include <iostream>

std::shared_ptr<ITransport> globalTransport;

int main(int argc, char** argv) {
  std::string url = "wss://echo.websocket.org";  // example:   ws://127.0.0.1:9001
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
    std::cout << "Connected to server" << std::endl;
  };
  globalTransport->onError = [](std::string_view errorMsg) {
    std::cerr << "Failed to connect: " << errorMsg << std::endl;
  };
  globalTransport->onText = [weak](std::string_view msg) {
    if (auto transport = weak.lock()) {
      std::cout << "Recv: " << msg << std::endl;
      std::string initialMsg = "Hello from server!";
      transport->sendText(initialMsg);
      std::cout << "Sent: " << initialMsg << std::endl;
    }
  };
  globalTransport->connect(url);

#ifdef __EMSCRIPTEN__
  // There is no possible to pause the program in a browser - this case browser will freeze.
  // For this tiny example allow a transport object to live.
  // Because the browser still works with this socket.
#else
  std::cout << "Press any key to exit" << std::endl;
  std::getchar();

  // ---------------------------------------
  // Closing connection
  // ---------------------------------------
  globalTransport.reset();
  std::cout << "Disconnected from server" << std::endl;
  return 0;
#endif

  std::cout << "main() done" << std::endl;
}