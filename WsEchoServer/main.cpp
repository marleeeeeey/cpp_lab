#include <App.h>
#include <spdlog/spdlog.h>

#include <array>
#include <memory>
#include <string>
#include <string_view>

#include "GameMessageTypes/GameMessageTypes.h"
#include "GetClientIpText.h"
#include "NetworkDataHandler/INetworkDataHandler.h"

// User data per socket
struct PerSocketData {
  std::string name;
  std::string clientIp;
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

std::string_view wsCloseCodeToText(int code) {
  // Source: https://github.com/Luka967/websocket-close-codes
  // clang-format off
  switch (code) {
    case 1000: return "Normal closure";
    case 1001: return "Going away";
    case 1002: return "Protocol error";
    case 1003: return "Unsupported Data";
    case 1004: return "UNUSED";
    case 1005: return "No Status Rcvd";
    case 1006: return "Abnormal Closure";
    case 1007: return "Invalid frame payload data";
    case 1008: return "Policy Violation";
    case 1009: return "Message Too Big";
    case 1010: return "Mandatory Ext.";
    case 1011: return "Internal Error";
    case 1012: return "Service Restart";
    case 1013: return "Try Again Later";
    case 1014: return "Bad gateway";
    case 1015: return "TLS handshake";
    case 3000: return "Unauthorized";
    case 3003: return "Forbidden";
    case 3008: return "Timeout";
    default:   return "WebSocket closed";
  }
  // clang-format on
}

std::string convertToKnownIp(const std::string_view ip) {
  // clang-format off
  if (ip == "127.0.0.1") return "localhost";
  if (ip == "0000:0000:0000:0000:0000:ffff:7f00:0001") return "localhost";
  if (ip == "0000:0000:0000:0000:0000:0000:0000:0001") return "loopback";
  return std::string(ip);
  // clang-format on
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

class ServerState {
  const std::string_view broadcastTopicName = "broadcast";
  int numberOfClients = 0;
  int maxNumberOfClients = 0;

 public:
  void incrementNumberOfClients() {
    numberOfClients++;
    if (numberOfClients > maxNumberOfClients) {
      maxNumberOfClients = numberOfClients;
    }
    SPDLOG_INFO("Number of clients (++): {}. Max number of clients: {}", numberOfClients, maxNumberOfClients);
  }

  void decrementNumberOfClients() {
    numberOfClients--;
    SPDLOG_INFO("Number of clients (--): {}. Max number of clients: {}", numberOfClients, maxNumberOfClients);
  }

  const std::string_view& getBroadcastTopicName() const { return broadcastTopicName; }
};

int main(int argc, char** argv) {
  // Example:
  // [2026-02-03 03:43:25.044] [info] WsEchoServer starting...
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

  SPDLOG_INFO("");
  SPDLOG_INFO("------------------------");
  SPDLOG_INFO("WsEchoServer starting...");
  SPDLOG_INFO("------------------------");
  SPDLOG_INFO("");

  int port = getPortFromCommandLine(argc, argv);

  auto app = std::make_shared<uWS::App>();

  auto state = std::make_shared<ServerState>();

  auto networkDataHandler = INetworkDataHandler::create();

  // -------------------------------------------
  // Start listening and message loop on connect
  // -------------------------------------------

  app->ws<PerSocketData>("/*",
                         {//
                          //
                          // ---------
                          // Upgrade
                          // ---------

                          .upgrade = [](uWS::HttpResponse<false>* response, uWS::HttpRequest* request, struct us_socket_context_t* context) {
                            response->onAborted([]() { SPDLOG_WARN("upgrade aborted by client"); });
                            auto socketIpText = response->getRemoteAddressAsText();
                            const auto clientIpRaw = getClientIpText(request, socketIpText);

                            // Calculate human readable name and known ip version
                            auto clientIp = convertToKnownIp(clientIpRaw);
                            auto humanName = humanhash::shortHumanName(clientIpRaw);
                            SPDLOG_INFO("ws.upgrade. clientIp={}, setName={}, socketIp={}, clientIpRaw={}, ",
                              clientIp, humanName, socketIpText, clientIpRaw);

                            // Create and move PerSocketData to .open method
                            PerSocketData perSocketData;
                            perSocketData.clientIp = clientIp;
                            perSocketData.name = humanName;
                            response->upgrade<PerSocketData>(
                                std::move(perSocketData),
                                request->getHeader("sec-websocket-key"),         // required
                                request->getHeader("sec-websocket-protocol"),    // optional
                                request->getHeader("sec-websocket-extensions"),  // optional
                                context); },

                          // ---------
                          // Open
                          // ---------

                          .open = [state](auto* ws) {
                            PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();
                            ws->subscribe(state->getBroadcastTopicName());
                            SPDLOG_INFO("ws.open");
                            state->incrementNumberOfClients(); },

                          // ---------
                          // Message
                          // ---------

                          .message = [app, state, &networkDataHandler](auto* ws, std::string_view msg, uWS::OpCode op) {
                            PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();
                            std::string personWithMessage = perSocketData->name + ": " + std::string(msg);
                            std::vector<uint8_t> message = networkDataHandler->prepareMessage(GMT_TextMessage, personWithMessage);
                            std::string_view messageStringView(reinterpret_cast<const char*>(message.data()), message.size());
                            app->publish(state->getBroadcastTopicName(), messageStringView, op, false);  // broadcast
                            SPDLOG_INFO("ws.message. {}", personWithMessage); },

                          // ---------
                          // Close
                          // ---------

                          .close = [state](auto* ws, int code, std::string_view msg) {
                            PerSocketData *perSocketData = (PerSocketData *) ws->getUserData();
                            msg = msg.empty() ? wsCloseCodeToText(code) : msg;
                            SPDLOG_INFO("ws.close. ip={}, name={}, code={}, msg={}",
                              perSocketData->clientIp, perSocketData->name, code, msg);
                            state->decrementNumberOfClients(); }});

  app->listen(port, [port](auto* token) {
    if (token) {
      SPDLOG_INFO("Listening on port {}", port);
    } else {
      SPDLOG_ERROR("Failed to listen on port {}", port);
      std::exit(1);
    }
  });

  app->run();
}
