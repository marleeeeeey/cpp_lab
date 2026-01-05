cmake_minimum_required(VERSION 3.20)
project(cpp_lab)

set(CMAKE_CXX_STANDARD 23)

# Add path to libraries.
get_filename_component(PATH_TO_LIBRARIES "../3rd_party/external_install/${CMAKE_BUILD_TYPE}" ABSOLUTE)
if (NOT EXISTS ${PATH_TO_LIBRARIES})
    message(FATAL_ERROR "Directory with dependencies not found at: ${PATH_TO_LIBRARIES}.
        Please clone and build libraries from https://github.com/marleeeeeey/3rd_party")
endif ()
list(APPEND CMAKE_PREFIX_PATH ${PATH_TO_LIBRARIES})

add_subdirectory(std_filesystem)
add_subdirectory(chat_engine_a)
add_subdirectory(game_engine_b)
add_subdirectory(debug_log_lib)
add_subdirectory(network_connection_lib)
add_subdirectory(chat_network_lib)

add_custom_target(build_all_projects)
add_dependencies(build_all_projects
        std_filesystem
        chat_engine_a
        game_engine_b
        debug_log_lib
        network_connection_lib
        chat_network_lib
)