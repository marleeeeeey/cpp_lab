add_executable(std_filesystem
        main.cpp
)

target_link_libraries(std_filesystem PRIVATE GTest::gtest_main)
