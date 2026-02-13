# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_library(AutoReconnectionNetwork
        src/AutoReconnectionNetwork.cpp
        src/AutoReconnectionNetworkFactory.cpp
        src/ReconnectPolicy.cpp
)

# -----------------------------------------------------
# Include
# -----------------------------------------------------

target_include_directories(AutoReconnectionNetwork
        PUBLIC include
        PRIVATE src
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(AutoReconnectionNetwork PRIVATE
        DoubleQueueNetwork
        spdlog::spdlog
        magic_enum::magic_enum
)
