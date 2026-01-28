cmake_minimum_required(VERSION 3.20)
project(cpp_lab)


# ----------------------------
# Options
# ----------------------------

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif ()

set(CPP_LAB_ENABLE_OBSOLETE_PROJECTS OFF CACHE BOOL "Enable/Disable obsolete projects from ALL target to speedup build")


# ----------------------------
# External
# ----------------------------

if (NOT CPP_LAB_ENABLE_OBSOLETE_PROJECTS)
    set(3RD_PARTY_SKIP "tracy;openal-soft;box2d;EnTT;implot;SDL3_image;spdlog;enet;cxxopts;nlohmann_json;asio" CACHE STRING "" FORCE)
endif ()
add_subdirectory(3rd_party)


# ----------------------------
# Internal
# ----------------------------

add_subdirectory(std)
add_subdirectory(game_engine_b)
add_subdirectory(debug_log_lib)
add_subdirectory(transport_layer)
add_subdirectory(transport_layer_client)

# Disable projects with Asio, OpenSSL and uWebSockets for Emscripten
if (NOT EMSCRIPTEN)

    if (CPP_LAB_ENABLE_OBSOLETE_PROJECTS)
        add_subdirectory(obsolete/asio_projects)
    endif ()

    add_subdirectory(web_sockets_echo_server_no_ssl)

endif ()