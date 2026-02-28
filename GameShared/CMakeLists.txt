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
        spdlog::spdlog
        PUBLIC # TODO: remove it
        glm::glm
)
