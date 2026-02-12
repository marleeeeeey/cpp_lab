# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_library(DoubleQueueNetwork
        src/DoubleQueueNetwork.cpp
        src/DoubleQueueNetworkFactory.cpp
        src/CanUseThreads.cpp
)

# -----------------------------------------------------
# Include
# -----------------------------------------------------

target_include_directories(DoubleQueueNetwork
        PUBLIC include
        PRIVATE src
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(DoubleQueueNetwork PRIVATE
        NetworkTransport
        spdlog::spdlog
        concurrentqueue
        magic_enum::magic_enum
)
