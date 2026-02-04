if (NOT DEFINED src OR NOT DEFINED dst)
    message(FATAL_ERROR "CopySingleIfExists.cmake: missing -Dsrc or -Ddst")
endif ()

if (EXISTS "${src}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${src}" "${dst}")
endif ()