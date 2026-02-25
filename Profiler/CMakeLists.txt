add_library(Profiler INTERFACE)

option(CPP_LAB_ENABLE_PROFILER "Enable CPP_LAB profiler" OFF)

target_include_directories(Profiler INTERFACE
        include
)

# Enable Tracy only on native builds AND when the option is ON.
if (NOT EMSCRIPTEN AND CPP_LAB_ENABLE_PROFILER)
    target_link_libraries(Profiler INTERFACE
            Tracy::TracyClient
    )
    target_compile_definitions(Profiler INTERFACE
            PROFILER_ENABLED=1
            TRACY_ENABLE
    )
else ()
    target_compile_definitions(Profiler INTERFACE
            PROFILER_ENABLED=0
    )
endif ()