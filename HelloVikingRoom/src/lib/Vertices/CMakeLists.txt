# Specify the libraries in this directory
add_library(VertexLib Vertex.cpp)
# Set the include directories for these libraries:
target_include_directories(VertexLib PUBLIC
                           "${INCLUDE_DIRS}")
# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VertexLib)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
