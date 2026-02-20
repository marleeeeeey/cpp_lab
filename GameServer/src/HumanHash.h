#pragma once
#include <string>
#include <string_view>

class HumanHash {
 public:
  // Hash key to a short human-readable string. Example: "MilaRaven42"
  static std::string getShortHumanName(std::string_view key);
};
