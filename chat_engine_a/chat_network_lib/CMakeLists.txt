add_library(chat_network_lib STATIC
        src/ChatServer.cpp
        src/ChatServerImpl.cpp
)

target_include_directories(chat_network_lib
        PUBLIC
        include
        PRIVATE
        src
        include/chat_network
)

find_package(asio REQUIRED)

target_link_libraries(chat_network_lib PUBLIC
        asio::asio
        debug_log_lib
)
