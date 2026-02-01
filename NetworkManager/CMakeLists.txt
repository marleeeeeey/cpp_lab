# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_library(NetworkManager
        src/NetworkManager.cpp
        src/NetworkManagerFactory.cpp
        src/CanUseThreads.cpp
)

# -----------------------------------------------------
# Include
# -----------------------------------------------------

target_include_directories(NetworkManager
        PUBLIC include
        PRIVATE src
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(NetworkManager PRIVATE
        NetworkTransport
        DebugLog
        concurrentqueue
        magic_enum::magic_enum
)
