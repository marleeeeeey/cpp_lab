#include <App.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

// User data per socket
struct PerSocketData {
  std::string name;
  std::string ip;
};

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

namespace humanhash {

// FNV-1a 64-bit: fast and deterministic hash for strings
constexpr std::uint64_t fnv1a64(std::string_view s) noexcept {
  std::uint64_t h = 14695981039346656037ull;
  for (unsigned char c : s) {
    h ^= c;
    h *= 1099511628211ull;
  }
  return h;
}

// Example: "MilaRaven42"
inline std::string shortHumanName(std::string_view key) {
  static constexpr std::array<std::string_view, 32> first = {
      "Mila", "Artem", "Lena", "Nina", "Ilya", "Oleg", "Vera", "Dima",
      "Roma", "Kira", "Sasha", "Tanya", "Yana", "Max", "Pavel", "Kirill",
      "Mira", "Alina", "Sonya", "Liza", "Anya", "Sergey", "Denis", "Nastya",
      "Gleb", "Egor", "Polina", "Dasha", "Vlad", "Timur", "Igor", "Katya"};

  static constexpr std::array<std::string_view, 32> last = {
      "Raven", "Stone", "River", "Woods", "Fox", "Storm", "Vale", "North",
      "Hill", "Lake", "Dawn", "Frost", "Reed", "Blake", "Cedar", "Hawk",
      "Silver", "Birch", "Ember", "Crown", "Field", "Wolf", "Ash", "Glade",
      "Sparrow", "Peak", "Breeze", "Harbor", "Quartz", "Meadow", "Cobalt", "Rowan"};

  const std::uint64_t h = fnv1a64(key);

  const auto firstIdx = static_cast<std::size_t>(h & 31ull);
  const auto lastIdx = static_cast<std::size_t>((h >> 5) & 31ull);

  // Two numbers for the collision reduction with the same dictionaries
  const auto suffix = static_cast<unsigned>((h >> 10) % 100ull);

  std::string out;
  out.reserve(24);
  out.append(first[firstIdx]);
  out.append(last[lastIdx]);

  out.push_back(static_cast<char>('0' + (suffix / 10)));
  out.push_back(static_cast<char>('0' + (suffix % 10)));
  return out;
}

}  // namespace humanhash

std::string_view gBroadcastTopicName = "broadcast";

int main(int argc, char** argv) {
  std::cout << "" << std::endl;
  std::cout << "------------------------" << std::endl;
  std::cout << "WsEchoServer starting..." << std::endl;
  std::cout << "------------------------" << std::endl;
  std::cout << "" << std::endl;

  int port = getPortFromCommandLine(argc, argv);

  auto app = std::make_shared<uWS::App>();

  // -------------------------------------------
  // Start listening and message loop on connect
  // -------------------------------------------

  app->ws<PerSocketData>("/*",
                         {.open = [](auto* ws) {
                            PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();
                            perSocketData->ip = std::string(ws->getRemoteAddressAsText());
                            perSocketData->name = humanhash::shortHumanName(perSocketData->ip);
                            std::cout << "ws.open. ip=" << perSocketData->ip
                            << ". name=" << perSocketData->name << std::endl;
                            ws->subscribe(gBroadcastTopicName); },
                          .message = [app](auto* ws, std::string_view msg, uWS::OpCode op) {
                            PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();
                            std::string answer = perSocketData->name + ": " + std::string(msg);
                            app->publish(gBroadcastTopicName, answer, op, false); // broadcast
                            std::cout << "ws.message: " << answer << std::endl; }});

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
