# ---------
# Sources
# ---------

add_library(GameInputManager
        src/IGameInputManager.cpp
        src/GameInputManager.cpp
)


# ----------
# Include
# ----------


target_include_directories(GameInputManager
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(GameInputManager PRIVATE
        # User libraries

        # 3rd_party libraries
        imgui::imgui
        SDL3::SDL3
        spdlog::spdlog
        EnTT::EnTT
        magic_enum::magic_enum
)
