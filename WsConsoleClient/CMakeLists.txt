# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_executable(WsConsoleClient
        main.cpp
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(WsConsoleClient PRIVATE
        NetworkTransport
        spdlog::spdlog
        cxxopts
)


# ----------------------------------
# Archive for publication to itch.io
# ----------------------------------
if (EMSCRIPTEN)
    include("${CPP_LAB_ROOT}/ItchWebPack.cmake")
    itch_pack_emscripten_web(
            TARGET WsConsoleClient
            ZIP_NAME "WsConsoleClient_itch_web.zip"
    )
endif ()
