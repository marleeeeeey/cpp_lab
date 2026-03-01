# ---------
# Sources
# ---------

add_library(GameUtils
        src/GameTimer.cpp
)


# ----------
# Include
# ----------

target_include_directories(GameUtils
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(GameUtils PRIVATE
        spdlog::spdlog
)
