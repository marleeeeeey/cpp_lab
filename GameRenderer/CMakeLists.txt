# ---------
# Sources
# ---------

add_library(GameRenderer
        src/IRenderContainer.cpp
        src/RenderContainer.cpp

        src/IChatRenderer.cpp
        src/ChatRenderer.cpp

        src/IGameWorldRenderer.cpp
        src/GameWorldRenderer.cpp

        src/IDebugRender.cpp
        src/DebugRender.cpp

        src/TimeUtils.cpp
)


# ----------
# Include
# ----------


target_include_directories(GameRenderer
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(GameRenderer PRIVATE
        # User libraries
        GameSharedObjects
        Profiler
        GameUtils

        # 3rd_party libraries
        imgui::imgui
        SDL3::SDL3
        glm::glm
        spdlog::spdlog
        EnTT::EnTT
        magic_enum::magic_enum
)
