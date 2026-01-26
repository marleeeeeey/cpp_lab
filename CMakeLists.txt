cmake_minimum_required(VERSION 3.20)
project(cpp_lab)

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()


# ----------------------------
# External
# ----------------------------

add_subdirectory(3rd_party)


# ----------------------------
# Internal
# ----------------------------

add_subdirectory(std)
add_subdirectory(game_engine_b)
add_subdirectory(debug_log_lib)
add_subdirectory(transport_layer)
add_subdirectory(transport_layer_client)

add_custom_target(build_all_projects)
add_dependencies(build_all_projects
        std_filesystem
        game_engine_b
        debug_log_lib
        transport_layer
        transport_layer_client
)

# Disable examples with Asio, OpenSSL and uWebSockets for Emscripten
if (NOT EMSCRIPTEN)
    add_subdirectory(chat_engine_a)
    add_subdirectory(network_connection_lib) # TODO make this layer buildable for Emscripten
    add_subdirectory(chat_network_lib)

    add_dependencies(build_all_projects
            chat_engine_a
            network_connection_lib
            chat_network_lib
    )
endif ()