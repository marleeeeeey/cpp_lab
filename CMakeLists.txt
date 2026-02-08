cmake_minimum_required(VERSION 3.20)
project(cpp_lab)


# ----------------------------
# Options
# ----------------------------

add_executable(CppLabWeb ALIAS GameEngine) # "CppLabWeb" target is used for GitHub pages build

if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # Enable std::thread / pthreads in WASM
    add_compile_options(-pthread)
    add_link_options(-pthread)
endif ()

set(CPP_LAB_ENABLE_OBSOLETE_PROJECTS OFF CACHE BOOL "Enable/Disable obsolete projects from ALL target to speedup build")

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

if (NOT CPP_LAB_ENABLE_OBSOLETE_PROJECTS)
    set(3RD_PARTY_SKIP "openal-soft;box2d;implot;SDL3_image;enet;nlohmann_json;asio;flatbuffers" CACHE STRING "" FORCE)
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
add_subdirectory(Profiler)

# Disable projects with Asio, OpenSSL and uWebSockets for Emscripten
if (NOT EMSCRIPTEN)

    if (CPP_LAB_ENABLE_OBSOLETE_PROJECTS)
        add_subdirectory(obsolete/asio_projects)
    endif ()

    add_subdirectory(WsEchoServer)

endif ()