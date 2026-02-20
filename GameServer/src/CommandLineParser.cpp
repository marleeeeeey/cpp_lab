#include "CommandLineParser.h"

#include <cstdlib>
#include <string_view>

CommandLineParser::CommandLineParser(int argc, char** argv) {
  // Example of usage:
  // GameServer.exe --port 8083
  int port = 9001;  // default
  for (int i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--port" && i + 1 < argc) {
      port = std::atoi(argv[i + 1]);
    }
  }
  port_ = port;
}

int CommandLineParser::getPort() const {
  return port_;
}