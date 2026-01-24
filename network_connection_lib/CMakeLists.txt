add_library(network_connection_lib STATIC
        src/NetworkConnection.cpp
)

target_include_directories(network_connection_lib
        PUBLIC include
        PRIVATE src
)

target_link_libraries(network_connection_lib PUBLIC
        asio::asio
        debug_log_lib
)
