add_executable(std_filesystem
        main.cpp
)

find_package(GTest REQUIRED)

target_link_libraries(std_filesystem PRIVATE GTest::gtest_main)
