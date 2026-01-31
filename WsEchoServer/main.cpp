#include <App.h>

#include <iostream>

int main(int argc, char** argv) {
  // ---------------------------
  // Read command line arguments
  // ---------------------------
  int port = 9001;  // default
  for (int i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--port" && i + 1 < argc) {
      port = std::atoi(argv[i + 1]);
    }
  }

  // --------------------
  // User data per socket
  // --------------------
  struct PerSocketData {};

  // -------------------------------------------
  // Start listening and message loop on connect
  // -------------------------------------------
  uWS::App()
      .ws<PerSocketData>("/*", {.open = [](auto* ws) {
                                    std::cout << "ws.open\n";
                                    ws->send("Hello from server!", uWS::OpCode::TEXT); },
                                .message = [](auto* ws, std::string_view msg, uWS::OpCode op) {
                                    ws->send(msg, op);
                                    std::cout << "ws.message: " << msg << std::endl; }})
      .listen(port, [port](auto* token) {
        if (token) {
          std::cout << "Listening on port " << port << std::endl;
        } else {
          std::cerr << "Failed to listen on port " << port << std::endl;
          std::exit(1);
        }
      })
      .run();
}
