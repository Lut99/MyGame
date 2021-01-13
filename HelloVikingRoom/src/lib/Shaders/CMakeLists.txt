# Define the custom commands to compile the shaders
# add_custom_command(OUTPUT
#     ${PROJECT_BINARY_DIR}/vert.spv
#     COMMAND glslc -v -o ${PROJECT_BINARY_DIR}/vert.spv ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert
#     COMMENT "   Building vertex shader..."
# )
# add_custom_command(OUTPUT
#     ${PROJECT_BINARY_DIR}/frag.spv
#     COMMAND glslc -v -o ${PROJECT_BINARY_DIR}/frag.spv ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag
#     COMMENT "   Building fragment shader..."
# )
