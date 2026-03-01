# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_library(NetworkDataHandler
        src/NetworkDataHandler.cpp
        src/INetworkDataHandler.cpp
)

# -----------------------------------------------------
# Include
# -----------------------------------------------------

target_include_directories(NetworkDataHandler
        PUBLIC include
        PRIVATE src
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(NetworkDataHandler PRIVATE
        # User Libs
        GlobalTypes

        # 3rd Party Libs
        spdlog::spdlog
)
