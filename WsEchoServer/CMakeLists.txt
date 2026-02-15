cmake_minimum_required(VERSION 3.20)
project(WsEchoServer)

add_executable(WsEchoServer
        main.cpp
        GetClientIpText.cpp
)

target_link_libraries(WsEchoServer PRIVATE
        # User libraries
        NetworkDataHandler
        GameMessageTypes

        # 3rd_party libraries
        uWebSockets::uWebSockets
        spdlog::spdlog
        magic_enum::magic_enum
)
