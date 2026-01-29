# -----------------------------------------------------------------
# Method to pack build artifact to publish as itch.io browser game.
#
# How to use:
#
#   include("${CPP_LAB_ROOT}/ItchWebPack.cmake")
#   itch_pack_emscripten_web(
#           TARGET transport_layer_client
#           ZIP_NAME "transport_layer_client_web.zip"
#   )
#
# Creates:
#
#   ./build-emscripten/transport_layer_client/transport_layer_client_web.zip
#
# -----------------------------------------------------------------
function(itch_pack_emscripten_web)
    set(options)
    set(oneValueArgs TARGET HTML_NAME JS_NAME WASM_NAME TEMP_DIR ZIP_NAME)
    set(multiValueArgs)
    cmake_parse_arguments(ITCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT EMSCRIPTEN)
        message(FATAL_ERROR "itch_pack_emscripten_web: Expected Emscripten build")
    endif ()

    if (NOT ITCH_TARGET)
        message(FATAL_ERROR "itch_pack_emscripten_web: TARGET is required")
    endif ()

    if (NOT ITCH_ZIP_NAME)
        message(FATAL_ERROR "itch_pack_emscripten_web: ZIP_NAME is required")
    endif ()

    # Defaults
    if (NOT ITCH_HTML_NAME)
        set(ITCH_HTML_NAME "${ITCH_TARGET}.html")
    endif ()
    if (NOT ITCH_JS_NAME)
        set(ITCH_JS_NAME "${ITCH_TARGET}.js")
    endif ()
    if (NOT ITCH_WASM_NAME)
        set(ITCH_WASM_NAME "${ITCH_TARGET}.wasm")
    endif ()
    if (NOT ITCH_TEMP_DIR)
        set(ITCH_TEMP_DIR "${CMAKE_CURRENT_BINARY_DIR}/temp_itch_web")
    endif ()

    set(_zip_path "${CMAKE_CURRENT_BINARY_DIR}/${ITCH_ZIP_NAME}")

    add_custom_command(TARGET "${ITCH_TARGET}" POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${ITCH_TEMP_DIR}"

            # rename html -> index.html
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/${ITCH_HTML_NAME}"
            "${ITCH_TEMP_DIR}/index.html"

            # copy js
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/${ITCH_JS_NAME}"
            "${ITCH_TEMP_DIR}/${ITCH_JS_NAME}"

            # copy wasm
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/${ITCH_WASM_NAME}"
            "${ITCH_TEMP_DIR}/${ITCH_WASM_NAME}"

            # zip files
            COMMAND "${CMAKE_COMMAND}" -E echo "Creating ${_zip_path} from ${ITCH_TEMP_DIR} content"
            COMMAND "${CMAKE_COMMAND}" -E chdir "${ITCH_TEMP_DIR}"
            "${CMAKE_COMMAND}" -E tar "cfv" "${_zip_path}" --format=zip -- .

            # cleanup temp dir
            COMMAND "${CMAKE_COMMAND}" -E rm -rf "${ITCH_TEMP_DIR}"

            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            VERBATIM
    )
endfunction()