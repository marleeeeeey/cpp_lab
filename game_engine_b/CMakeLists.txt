add_executable(game_engine_b
        main.cpp
        AppInstance.cpp
)

find_package(SDL3 REQUIRED)

target_link_libraries(game_engine_b PRIVATE SDL3::SDL3)

if (WIN32)
    add_custom_command(TARGET game_engine_b POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:game_engine_b>)
else ()
    message(WARNING "Place here a code to copy SDL3 shared library for your system")
endif ()