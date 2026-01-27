add_executable(std
        main.cpp
)

target_link_libraries(std PRIVATE GTest::gtest_main)
