# Specify the libraries in this directory
add_library(RenderPassLib SquarePass.cpp)
# Set the include directories for these libraries:
target_include_directories(RenderPassLib PUBLIC
                           "${INCLUDE_DIRS}")
# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS RenderPassLib)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
