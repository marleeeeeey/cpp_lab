#include <App.h>

#include <iostream>
#include <memory>

// User data per socket
struct PerSocketData {};

namespace {
int getPortFromCommandLine(int argc, char** argv) {
  // Example of usage:
  // WsEchoServer.exe --port 8083
  int port = 9001;  // default
  for (int i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--port" && i + 1 < argc) {
      port = std::atoi(argv[i + 1]);
    }
  }
  return port;
}
}  // namespace

std::string_view gBroadcastTopicName = "broadcast";

int main(int argc, char** argv) {
  int port = getPortFromCommandLine(argc, argv);

  auto app = std::make_shared<uWS::App>();

  // -------------------------------------------
  // Start listening and message loop on connect
  // -------------------------------------------
  app->ws<PerSocketData>("/*", {.open = [](auto* ws) {
                                  std::cout << "ws.open\n";
                                  ws->subscribe(gBroadcastTopicName); },
                                .message = [app](auto* ws, std::string_view msg, uWS::OpCode op) {
                                  // ws->send(msg, op); // send back
                                  app->publish(gBroadcastTopicName, msg, op, false); // broadcast
                                  std::cout << "ws.message: " << msg << std::endl; }});

  app->listen(port, [port](auto* token) {
    if (token) {
      std::cout << "Listening on port " << port << std::endl;
    } else {
      std::cerr << "Failed to listen on port " << port << std::endl;
      std::exit(1);
    }
  });

  app->run();
}
