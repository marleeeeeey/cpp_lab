# ---------
# Sources
# ---------

add_library(UserInputManager
        src/IUserInputManager.cpp
        src/UserInputManager.cpp
)


# ----------
# Include
# ----------


target_include_directories(UserInputManager
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(UserInputManager PRIVATE
        # User libraries

        # 3rd_party libraries
        imgui::imgui
        SDL3::SDL3
        spdlog::spdlog
        EnTT::EnTT
        magic_enum::magic_enum
)
