# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_library(transport_layer
        src/TransportFactory.cpp
)

if (EMSCRIPTEN)
    target_sources(transport_layer PRIVATE
            src/BrowserWebSocketTransport.cpp
    )
else ()
    target_sources(transport_layer PRIVATE
            src/DesktopWebSocketTransport.cpp
    )
endif ()


# -----------------------------------------------------
# Include
# -----------------------------------------------------

target_include_directories(transport_layer
        PUBLIC include
        PRIVATE src
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(transport_layer PRIVATE
        magic_enum::magic_enum
        debug_log_lib
)

if (NOT EMSCRIPTEN)
    target_link_libraries(transport_layer PRIVATE
            ixwebsocket::ixwebsocket
    )
endif ()

if (EMSCRIPTEN)
    target_link_options(transport_layer PUBLIC
            "SHELL:-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=\\$stringToNewUTF8"
            "SHELL:-sEXPORTED_FUNCTIONS=['_main','_free','_malloc','_ws_on_open','_ws_on_message','_ws_on_close','_ws_on_error']")
endif ()