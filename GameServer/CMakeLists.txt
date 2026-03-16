cmake_minimum_required(VERSION 3.20)
project(GameServer)

add_executable(GameServer
        src/ServerMain.cpp
        src/GetClientIpText.cpp
        src/CommandLineParser.cpp
        src/ServerState.cpp
        src/BinaryMessageParser.cpp
        src/ServerGameLoop.cpp
)

target_link_libraries(GameServer PRIVATE
        # User libraries
        NetworkDataHandler
        GameShared
        GameSerialization
        GlobalTypes
        GameUtils

        # 3rd_party libraries
        uWebSockets::uWebSockets
        spdlog::spdlog
        magic_enum::magic_enum
        # SDL3::SDL3 # SDL should not be used in Server!
)

include("${CPP_LAB_ROOT}/cmake/Profiler.cmake")
cpp_lab_enable_profiling(GameServer FALSE)
