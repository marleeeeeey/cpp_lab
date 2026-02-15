# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_library(DoubleQueueNetwork
        src/IDoubleQueueNetwork.cpp
        src/DoubleQueueNetwork.cpp
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
        # User libraries
        NetworkTransport

        # 3rd_party libraries
        spdlog::spdlog
        concurrentqueue
        magic_enum::magic_enum
)
