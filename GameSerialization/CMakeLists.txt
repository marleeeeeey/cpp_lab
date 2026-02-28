# ----------
# Sources
# ----------

add_library(GameSerialization
        src/GameSerialization.cpp
)

# --------------
# Flatbuffers
# --------------

include(${THIRD_PARTY_ROOT}/cmake/FlatbuffersHelper.cmake)

# This creates a target 'FlatbufferSchemas' that you can link to anywhere.
# Target allow you #include "Player_generated.h" to work with flatbuffers methods.
add_flatbuffers_schema(FlatbufferSchemas
        fbs/Bullet.fbs
        fbs/ChatMessage.fbs
        fbs/Math.fbs
        fbs/Player.fbs
        fbs/WorldSnapshot.fbs
)

# ----------
# Include
# ----------

target_include_directories(GameSerialization
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(GameSerialization PRIVATE
        # User libraries
        FlatbufferSchemas
        GameShared

        # 3rd_party libraries
        spdlog::spdlog
        magic_enum::magic_enum
        glm::glm
)
