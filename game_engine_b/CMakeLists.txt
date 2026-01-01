add_executable(game_engine_b
        src/main.cpp
        src/AppInstance.cpp
        src/GameWorld.cpp
        src/SceneRenderer.cpp
        src/UserInputManger.cpp
)

find_package(SDL3 REQUIRED)
find_package(imgui REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(game_engine_b PRIVATE
        imgui::imgui
        SDL3::SDL3
        glm::glm
)

if (WIN32)
    add_custom_command(TARGET game_engine_b POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:game_engine_b>)
else ()
    message(WARNING "Place here a code to copy SDL3 shared library for your system")
endif ()