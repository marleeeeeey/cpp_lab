#include "WsCloseCodeToText.h"

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
