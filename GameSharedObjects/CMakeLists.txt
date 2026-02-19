# ---------
# Sources
# ---------

add_library(GameSharedObjects INTERFACE
)


# ----------
# Include
# ----------

target_include_directories(GameSharedObjects INTERFACE
        include
)


# -------
# Link
# -------

target_link_libraries(GameSharedObjects INTERFACE
        # User libraries

        # 3rd_party libraries
        glm::glm
)
