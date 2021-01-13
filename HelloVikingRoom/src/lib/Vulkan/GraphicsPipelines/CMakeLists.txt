# Specify the libraries in this directory
add_library(GraphicsPipelineLib SquarePipeline.cpp)
# Set the include directories for these libraries:
target_include_directories(GraphicsPipelineLib PUBLIC
                           "${INCLUDE_DIRS}")
# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS GraphicsPipelineLib)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
