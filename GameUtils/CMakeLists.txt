# ---------
# Sources
# ---------

add_library(GameUtils
        src/GameTimer.cpp
        src/TimeUtils.cpp
        src/GeomUtils.cpp
        src/HumanHash.cpp
        src/WsCloseCodeToText.cpp
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
        # User Libraries
        GlobalTypes

        # 3rd Party Libraries
        spdlog::spdlog
        glm::glm
)
