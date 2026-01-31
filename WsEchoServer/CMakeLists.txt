cmake_minimum_required(VERSION 3.20)
project(WsEchoServer)

add_executable(WsEchoServer
        main.cpp
)

target_link_libraries(WsEchoServer PRIVATE
        uWebSockets::uWebSockets
)
