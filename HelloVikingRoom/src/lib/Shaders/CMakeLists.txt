# Define the custom commands to compile the shaders
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vert.spv
    COMMAND glslc -o ${CMAKE_CURRENT_BINARY_DIR}/vert.spv ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert
)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/frag.spv
  COMMAND glslc -o ${CMAKE_CURRENT_BINARY_DIR}/frag.spv ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag
)