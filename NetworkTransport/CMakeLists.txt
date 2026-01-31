# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_library(NetworkTransport
        src/TransportFactory.cpp
)

if (EMSCRIPTEN)
    target_sources(NetworkTransport PRIVATE
            src/BrowserWebSocketTransport.cpp
    )
else ()
    target_sources(NetworkTransport PRIVATE
            src/DesktopWebSocketTransport.cpp
    )
endif ()


# -----------------------------------------------------
# Include
# -----------------------------------------------------

target_include_directories(NetworkTransport
        PUBLIC include
        PRIVATE src
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(NetworkTransport PRIVATE
        magic_enum::magic_enum
        DebugLog
)

if (NOT EMSCRIPTEN)
    target_link_libraries(NetworkTransport PRIVATE
            ixwebsocket::ixwebsocket
    )
endif ()

if (EMSCRIPTEN)
    target_link_options(NetworkTransport PUBLIC
            "SHELL:-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=\\$stringToNewUTF8"
            "SHELL:-sEXPORTED_FUNCTIONS=['_main','_free','_malloc','_ws_on_open','_ws_on_message','_ws_on_close','_ws_on_error']")
endif ()