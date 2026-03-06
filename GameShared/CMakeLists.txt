# ---------
# Sources
# ---------

add_library(GameShared
        src/Simulation.cpp
)


# ----------
# Include
# ----------

target_include_directories(GameShared
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(GameShared
        PRIVATE
        # User Libraries
        GameUtils

        # 3rd Party Libraries
        spdlog::spdlog

        PUBLIC
        glm::glm # OK to public glm dependency and use it in interfaces
)
