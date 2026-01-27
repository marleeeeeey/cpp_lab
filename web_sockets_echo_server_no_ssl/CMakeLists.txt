cmake_minimum_required(VERSION 3.20)
project(web_sockets_echo_server_no_ssl)

add_executable(web_sockets_echo_server_no_ssl
        main.cpp
)

target_link_libraries(web_sockets_echo_server_no_ssl PRIVATE
        uWebSockets::uWebSockets
)
