# ----------
# Sources
# ----------

add_library(SerializationProtocol
        src/SerializationProtocol.cpp
)

# --------------
# Flatbuffers
# --------------

include(${THIRD_PARTY_ROOT}/FlatbuffersHelper.cmake)

# This creates a target 'PlayerFlatbuffer' that you can link to anywhere.
# Target allow you #include "Player_generated.h" to work with flatbuffers methods.
add_flatbuffers_schema(PlayerFlatbuffer
        fbs/Player.fbs
)

# ----------
# Include
# ----------

target_include_directories(SerializationProtocol
        PUBLIC include
        PRIVATE src
)


# -------
# Link
# -------

target_link_libraries(SerializationProtocol PRIVATE
        # User libraries
        PlayerFlatbuffer

        # 3rd_party libraries
        spdlog::spdlog
        magic_enum::magic_enum
)
