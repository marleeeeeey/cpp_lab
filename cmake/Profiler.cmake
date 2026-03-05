option(CPP_LAB_ENABLE_PROFILER "Allow CPP profiler for any target" OFF)

# Usage:
# include("${CPP_LAB_ROOT}/cmake/Profiler.cmake")
# cpp_lab_enable_profiling(GameServer FALSE)
function(cpp_lab_enable_profiling target_name enabled)
    target_link_libraries(${target_name} PRIVATE
            Profiler
    )

    if (enabled AND NOT EMSCRIPTEN AND CPP_LAB_ENABLE_PROFILER)
        target_link_libraries(${target_name} PRIVATE
                Tracy::TracyClient
        )
        target_compile_definitions(${target_name} PRIVATE
                PROFILER_ENABLED=1
                TRACY_ENABLE
        )
    else ()
        target_compile_definitions(${target_name} PRIVATE
                PROFILER_ENABLED=0
        )
    endif ()
endfunction()