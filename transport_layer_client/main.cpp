#include <transport_layer/TransportFactory.h>

#include <chrono>
#include <future>
#include <iostream>

int main() {
  // ---------------------------------------
  // Get transport interface
  // ---------------------------------------
  std::shared_ptr<ITransport> transport = createTransport();

  // ---------------------------------------
  // Initiate connection and message loop
  // ---------------------------------------
  std::string url = "wss://echo.websocket.org";
  transport->onOpen = []() {
    std::cout << "Connected to server" << std::endl;
  };
  transport->onError = [](std::string_view errorMsg) {
    std::cerr << "Failed to connect: " << errorMsg << std::endl;
  };
  transport->onText = [transport](std::string_view msg) {
    std::cout << "Recv: " << msg << std::endl;
    std::string initialMsg = "Hello from server!";
    transport->sendText(initialMsg);
    std::cout << "Sent: " << initialMsg << std::endl;
  };
  transport->connect(url);

#ifndef __EMSCRIPTEN__
  std::cout << "Press any key to exit" << std::endl;
  std::getchar();

  // ---------------------------------------
  // Closing connection
  // ---------------------------------------
  std::cout << "Disconnected from server" << std::endl;
  return 0;
#endif
}