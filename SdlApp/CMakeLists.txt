# ---------
# Sources
# ---------

add_library(SdlApp
        src/GameMain.cpp
        src/SdlWithImGuiWrapper.cpp
)


# ----------
# Include
# ----------


target_include_directories(SdlApp
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(SdlApp PRIVATE
        # User libraries
        Profiler

        # 3rd_party libraries
        imgui::imgui
        SDL3::SDL3
        spdlog::spdlog
        EnTT::EnTT
        magic_enum::magic_enum
)
