cmake_minimum_required(VERSION 3.20)
project(cpp_lab)


# ----------------------------
# Options
# ----------------------------

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # Enable std::thread / pthreads in WASM
    add_compile_options(-pthread)
    add_link_options(-pthread)
endif ()

set(CPP_LAB_ENABLE_OBSOLETE_PROJECTS OFF CACHE BOOL "Enable/Disable obsolete projects from ALL target to speedup build")

set(CPP_LAB_ROOT "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "cpp_lab root directory")


# ----------------------------
# External
# ----------------------------

if (NOT CPP_LAB_ENABLE_OBSOLETE_PROJECTS)
    set(3RD_PARTY_SKIP "tracy;openal-soft;box2d;EnTT;implot;SDL3_image;enet;nlohmann_json;asio" CACHE STRING "" FORCE)
endif ()
add_subdirectory(3rd_party)


# ----------------------------
# Internal
# ----------------------------

add_subdirectory(CodeSamples)
add_subdirectory(GameEngine)
add_subdirectory(DebugLog)
add_subdirectory(NetworkTransport)
add_subdirectory(NetworkManager)
add_subdirectory(WsConsoleClient)

# Disable projects with Asio, OpenSSL and uWebSockets for Emscripten
if (NOT EMSCRIPTEN)

    if (CPP_LAB_ENABLE_OBSOLETE_PROJECTS)
        add_subdirectory(obsolete/asio_projects)
    endif ()

    add_subdirectory(WsEchoServer)

endif ()