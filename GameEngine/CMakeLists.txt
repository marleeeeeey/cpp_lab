add_executable(GameEngine
        src/main.cpp
        src/AppInstance.cpp
        src/GameWorld.cpp
        src/SceneRenderer.cpp
        src/UserInputManger.cpp
        src/NetworkManager.cpp
        src/CanUseThreads.cpp
)

target_link_libraries(GameEngine PRIVATE
        imgui::imgui
        SDL3::SDL3
        glm::glm
        NetworkTransport
        DebugLog
        concurrentqueue
        cxxopts
        magic_enum::magic_enum
)

if (NOT EMSCRIPTEN) # Emscripten always use static libraries. Coping is not needed.
    if (WIN32)
        add_custom_command(TARGET GameEngine POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:GameEngine>)
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
            TARGET GameEngine
            ZIP_NAME "GameEngine_itch_web.zip"
    )
endif ()
