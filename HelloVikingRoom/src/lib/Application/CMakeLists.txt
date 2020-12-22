# Specify the libraries in this directory
add_library(MainWindow STATIC ${CMAKE_CURRENT_SOURCE_DIR}/MainWindow.cpp)

# Set the dependencies for this library:
target_include_directories(MainWindow PUBLIC
                           "${INCLUDE_DIRS}")

# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS MainWindow)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
