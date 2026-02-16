#pragma once

class CommandLineParser {
 public:
  CommandLineParser(int argc, char** argv);
  int getPort() const;

 private:
  int port_;
};
