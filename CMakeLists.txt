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

set(CPP_LAB_ROOT "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "cpp_lab root directory")

set(CPP_LAB_LOCAL_BUILD OFF CACHE BOOL "Enable/Disable specific compiler options for debug. Do not use for release build")

# ----------------------
# Preprocessor global
# ----------------------

if (CPP_LAB_LOCAL_BUILD)
    message(WARNING "CPP_LAB_LOCAL_BUILD enabled")
    add_compile_definitions(
            SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
    )
endif ()

# ----------------------------
# External
# ----------------------------

if (NOT 3RD_PARTY_BUILD_EXAMPLES)
    set(3RD_PARTY_SKIP "openal-soft;box2d;implot;SDL3_image;enet;nlohmann_json;asio" CACHE STRING "" FORCE)
endif ()
add_subdirectory(3rd_party)


# ----------------------------
# Internal
# ----------------------------

add_subdirectory(GameEngine)
add_subdirectory(GameInputManager)
add_subdirectory(GameMessageTypes)
add_subdirectory(GameRenderer)
add_subdirectory(GameSerialization)
add_subdirectory(GameSharedObjects)
add_subdirectory(Network/AutoReconnectionNetwork)
add_subdirectory(Network/DoubleQueueNetwork)
add_subdirectory(Network/NetworkDataHandler)
add_subdirectory(Network/NetworkTransport)
add_subdirectory(Profiler)
add_subdirectory(SdlApp)

# Disable projects with Asio, OpenSSL and uWebSockets for Emscripten
if (NOT EMSCRIPTEN)
    add_subdirectory(GameServer)
endif ()