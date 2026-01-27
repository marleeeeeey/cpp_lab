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


# -----------------------------------------------------
# Archive for publication to itch.io
# Creates achieve: \cmake-build-emscripten-debug\transport_layer_client\itch-web.zip
# -----------------------------------------------------

if (EMSCRIPTEN)
    set(ITCH_STAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/itch-web")
    set(ITCH_ZIP_PATH "${CMAKE_CURRENT_BINARY_DIR}/itch-web.zip")

    add_custom_command(TARGET transport_layer_client POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ITCH_STAGE_DIR}"

            # rename to index.html
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/transport_layer_client.html"
            "${ITCH_STAGE_DIR}/index.html"

            # copy js/wasm to index.html
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/transport_layer_client.js"
            "${ITCH_STAGE_DIR}/transport_layer_client.js"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/transport_layer_client.wasm"
            "${ITCH_STAGE_DIR}/transport_layer_client.wasm"

            # zip
            COMMAND ${CMAKE_COMMAND} -E echo "Creating ${ITCH_ZIP_PATH} from ${ITCH_STAGE_DIR} content"
            COMMAND ${CMAKE_COMMAND} -E chdir "${ITCH_STAGE_DIR}"
            ${CMAKE_COMMAND} -E tar cfv "${ITCH_ZIP_PATH}" --format=zip -- .

            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"

            VERBATIM
    )
endif ()