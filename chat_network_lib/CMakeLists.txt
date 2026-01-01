add_library(chat_network_lib STATIC
        src/ChatServer.cpp
        src/ClientSession.cpp
        src/ChatRoom.cpp
        src/ChatClient.cpp
)

target_include_directories(chat_network_lib
        PUBLIC include
        PRIVATE src
)

find_package(asio REQUIRED)

target_link_libraries(chat_network_lib PUBLIC
        asio::asio
        debug_log_lib
        network_connection_lib
)
