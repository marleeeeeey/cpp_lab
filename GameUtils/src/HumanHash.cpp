#include "GameUtils/HumanHash.h"

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
      "Mick", "David", "Freddie", "Elvis", "Stevie", "Eric", "Bob", "John",
      "Paul", "George", "Kurt", "Janis", "Jimi", "Aretha", "B.B.", "Ray",
      "Chuck", "Etta", "Little", "Bonnie", "Brian", "Roger", "Muddy", "James",
      "Ringo", "Debbie", "Billy", "Tom", "Phil", "Bruce", "Ozzy", "Lady"};

  static constexpr std::array<std::string_view, 32> last = {
      "Jagger", "Bowie", "Mercury", "Presley", "Wonder", "Clapton", "Dylan", "Lennon",
      "McCartney", "Harrison", "Cobain", "Joplin", "Hendrix", "Franklin", "King", "Charles",
      "Berry", "Houston", "Richard", "Raitt", "May", "Townshend", "Waters", "Brown",
      "Starr", "Hawkins", "Gaye", "Petty", "Gilmour", "Springsteen", "Osbourne", "Gaga"};

  const std::uint64_t h = fnv1a64(key);

  const auto firstIdx = static_cast<std::size_t>(h & 31ull);
  const auto lastIdx = static_cast<std::size_t>((h >> 5) & 31ull);

  std::string out;
  out.reserve(24);
  out.append(first[firstIdx]);
  out.append(last[lastIdx]);

  if constexpr (false) {
    // Two numbers for the collision reduction with the same dictionaries
    const auto suffix = static_cast<unsigned>((h >> 10) % 100ull);
    out.push_back(static_cast<char>('0' + (suffix / 10)));
    out.push_back(static_cast<char>('0' + (suffix % 10)));
  }

  return out;
}
