#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

TEST(std_filesystem, error_code) {
  std::error_code ec;
  auto result = std::filesystem::create_directory("unexisted/file/path_hierarchy", ec);
  EXPECT_FALSE(result);
  EXPECT_TRUE(ec);
  EXPECT_EQ(ec.value(), 3);
  EXPECT_EQ(ec.message(), "The system cannot find the path specified.");
}
