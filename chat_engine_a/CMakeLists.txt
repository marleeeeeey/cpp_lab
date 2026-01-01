add_executable(chat_engine_a
        src/main.cpp
        src/AppInstance.cpp
        src/ChatManager.cpp
        src/SceneRenderer.cpp
)

find_package(SDL3 REQUIRED)
find_package(imgui REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(chat_engine_a PRIVATE
        imgui::imgui
        SDL3::SDL3
        glm::glm
        chat_network_lib
)

if (WIN32)
    add_custom_command(TARGET chat_engine_a POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:chat_engine_a>)
else ()
    message(WARNING "Place here a code to copy SDL3 shared library for your system")
endif ()