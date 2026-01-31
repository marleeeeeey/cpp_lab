add_executable(CodeSamples
        main.cpp
)

target_link_libraries(CodeSamples PRIVATE GTest::gtest_main)
