add_subdirectory(asio_projects/network_connection_lib)
add_subdirectory(asio_projects/chat_network_lib)
add_subdirectory(asio_projects/chat_engine_a)

add_dependencies(build_all_projects
        chat_engine_a
        network_connection_lib
        chat_network_lib
)