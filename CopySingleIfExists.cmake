# ----------------------------------------------------------------
# Example of usage: copy data (optional: skip if missing)
# COMMAND "${CMAKE_COMMAND}"
# -Dsrc:FILEPATH=${CMAKE_CURRENT_BINARY_DIR}/${ITCH_DATA_NAME}
# -Ddst:FILEPATH=${ITCH_TEMP_DIR}/${ITCH_DATA_NAME}
# -P "${CPP_LAB_ROOT}/CopySingleIfExists.cmake"
# ----------------------------------------------------------------

if (NOT DEFINED src OR NOT DEFINED dst)
    message(FATAL_ERROR "CopySingleIfExists.cmake: missing -Dsrc or -Ddst")
endif ()

if (EXISTS "${src}")
    execute_process(COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${src}" "${dst}")
endif ()