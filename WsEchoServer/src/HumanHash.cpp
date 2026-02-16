#include "HumanHash.h"

#include <array>
#include <cstdint>

namespace {

// FNV-1a 64-bit: fast and deterministic hash for strings
constexpr std::uint64_t fnv1a64(std::string_view s) noexcept {
  std::uint64_t h = 14695981039346656037ull;
  for (unsigned char c : s) {
    h ^= c;
    h *= 1099511628211ull;
  }
  return h;
}
}  // namespace

std::string HumanHash::getShortHumanName(std::string_view key) {
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
