add_executable(game_engine_b
        src/main.cpp
        src/AppInstance.cpp
        src/GameWorld.cpp
        src/SceneRenderer.cpp
        src/UserInputManger.cpp
)

target_link_libraries(game_engine_b PRIVATE
        imgui::imgui
        SDL3::SDL3
        glm::glm
        transport_layer
        debug_log_lib
)

if (NOT EMSCRIPTEN) # Emscripten always use static libraries. Coping is not needed.
    if (WIN32)
        add_custom_command(TARGET game_engine_b POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:game_engine_b>)
    else ()
        message(WARNING "Place here a code to copy SDL3 shared library for your system")
    endif ()
endif ()


# ----------------------------------
# Archive for publication to itch.io
# ----------------------------------
if (EMSCRIPTEN)
    include("${CPP_LAB_ROOT}/ItchWebPack.cmake")
    itch_pack_emscripten_web(
            TARGET game_engine_b
            ZIP_NAME "game_engine_b_itch_web.zip"
    )
endif ()
