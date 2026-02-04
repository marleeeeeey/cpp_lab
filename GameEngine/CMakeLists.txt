# ----------------------
# Sources
# ----------------------

add_executable(GameEngine
        src/main.cpp
        src/GameApp.cpp
        src/GameWorld.cpp
        src/SceneRenderer.cpp
        src/UserInputManger.cpp
        src/ChatDataForRendering.cpp
)


# ----------------------
# Link
# ----------------------

target_link_libraries(GameEngine PRIVATE
        # User libraries
        NetworkManager

        # 3rd_party libraries
        imgui::imgui
        SDL3::SDL3
        glm::glm
        cxxopts
        spdlog::spdlog
)


# -----------------------------------
# Post build actions: copy libraries
# -----------------------------------

if (NOT EMSCRIPTEN) # Emscripten always use static libraries. Coping is not needed.
    if (WIN32)
        add_custom_command(TARGET GameEngine POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:GameEngine>
        )
    else ()
        message(WARNING "Place here a code to copy SDL3 shared library for your system")
    endif ()
endif ()



# -----------------------------------
# Post build actions: copy assets
# -----------------------------------

if (NOT EMSCRIPTEN)
    add_custom_command(TARGET GameEngine POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CPP_LAB_ROOT}/assets
            $<TARGET_FILE_DIR:GameEngine>/assets
    )
endif ()

if (EMSCRIPTEN)
    # For example:
    # Fontpath: /assets/fonts/OpenSans-VariableFont_wdth,wght.ttf should exist in VFS in runtime
    target_link_options(GameEngine PRIVATE
            "SHELL:--preload-file ${CPP_LAB_ROOT}/assets@/assets"
    )
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
