# -----------------------------------------------------
# Sources
# -----------------------------------------------------

add_executable(transport_layer_client
        main.cpp
)


# -----------------------------------------------------
# Link
# -----------------------------------------------------

target_link_libraries(transport_layer_client PRIVATE
        transport_layer
)
