# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_executable(transport_layer_client
        main.cpp
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(transport_layer_client PRIVATE
        transport_layer
)


# ---------------------------------------------------------------------------
# Archive for publication to itch.io
# Creates achieve: ./build-emscripten/itch_web/transport_layer_client_web.zip
# ---------------------------------------------------------------------------
if (EMSCRIPTEN)
    include("${CPP_LAB_ROOT}/ItchWebPack.cmake")
    itch_pack_emscripten_web(
            TARGET transport_layer_client
            ZIP_NAME "transport_layer_client_itch_web.zip"
    )
endif ()
