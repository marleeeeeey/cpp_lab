#include <gtest/gtest.h>

#include <filesystem>

TEST(std_filesystem, error_code) {
  std::error_code ec;
  auto result = std::filesystem::create_directory("unexisted/file/path_hierarchy", ec);
  EXPECT_FALSE(result);
  EXPECT_TRUE(ec);
  EXPECT_EQ(ec.value(), 3);
  EXPECT_EQ(ec.message(), "The system cannot find the path specified.");
}

struct Dog {
  std::string name;
};

template <typename T>
std::unique_ptr<T> makeType() {
  return std::make_unique<T>();
}

TEST(std_map, with_unique_ptr) {
  std::map<std::string, std::unique_ptr<Dog>> dogs;
  auto myDog = makeType<Dog>();
  myDog->name = "Fido";
  dogs.insert({"Fido", std::move(myDog)});
  EXPECT_EQ(dogs["Fido"]->name, "Fido");
}