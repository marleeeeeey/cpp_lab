#include "GetClientIpText.h"

#include <App.h>

#include <string>

#include "spdlog/spdlog.h"

std::string_view trim(std::string_view s) {
  while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.remove_prefix(1);
  while (!s.empty() && (s.back() == ' ' || s.back() == '\t')) s.remove_suffix(1);
  return s;
}

std::string_view takeUntil(std::string_view s, char delim) {
  size_t p = s.find(delim);
  return (p == std::string_view::npos) ? s : s.substr(0, p);
}

// X-Forwarded-For: "client, proxy1, proxy2"
// We want the first token (client). It can also contain spaces.
std::string firstXForwardedFor(std::string_view xff) {
  xff = trim(xff);
  if (xff.empty()) return {};
  std::string_view first = trim(takeUntil(xff, ','));
  // Sometimes it's quoted; keep it simple:
  if (first.size() >= 2 && first.front() == '"' && first.back() == '"') {
    first = first.substr(1, first.size() - 2);
  }
  return std::string(first);
}

bool isTrustedProxy(std::string_view socketIpText) {
  // Trust localhost only
  return socketIpText == "127.0.0.1" ||
         socketIpText == "::1" ||
         socketIpText == "0000:0000:0000:0000:0000:ffff:7f00:0001";
}

std::string getHeader(uWS::HttpRequest* req, std::string_view name) {
  // Copy to string
  auto v = req->getHeader(name);
  return std::string(v);
}

std::string getClientIpText(uWS::HttpRequest* req, std::string_view socketIpText) {
  if (!isTrustedProxy(socketIpText)) {
    SPDLOG_WARN("Untrusted proxy: {}", socketIpText);
    return std::string(socketIpText);
  }

  // 1) X-Forwarded-For
  std::string xff = getHeader(req, "x-forwarded-for");
  std::string ip = firstXForwardedFor(xff);
  if (!ip.empty()) return ip;

  // 2) X-Real-IP
  std::string xrip = getHeader(req, "x-real-ip");
  xrip = std::string(trim(xrip));
  if (!xrip.empty()) return xrip;

  // 3) fallback
  return std::string(socketIpText);
}