cmake_minimum_required(VERSION 3.20)
project(WsEchoServer)

add_executable(WsEchoServer
        src/ServerMain.cpp
        src/GetClientIpText.cpp
        src/WsCloseCodeToText.cpp
        src/HumanHash.cpp
        src/CommandLineParser.cpp
        src/ServerState.cpp
        src/BinaryMessageParser.cpp
)

target_link_libraries(WsEchoServer PRIVATE
        # User libraries
        NetworkDataHandler
        GameMessageTypes
        GameSharedObjects
        GameSerialization

        # 3rd_party libraries
        uWebSockets::uWebSockets
        spdlog::spdlog
        magic_enum::magic_enum
)
