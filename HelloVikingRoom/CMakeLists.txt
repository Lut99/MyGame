cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(HelloVikingRoom VERSION 1.0.0)

# If we're on Windows, prefix the prefix path
IF(WIN32)
list(APPEND CMAKE_PREFIX_PATH "C:/Program Files (x86)/GLFW/lib/cmake/glfw3")
ENDIF()

# Be sure that both Vulkan & GLFW are installed
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define all include directories
get_target_property(GLFW_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
SET(INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/src/lib" "${Vulkan_INCLUDE_DIRS}" "${GLFW_DIR}")

# Add the lib subdirectory
add_subdirectory(src)



##### HELLOVIKINGROOM TARGET #####
# Specify which file will compile to the executable
add_executable(hellovikingroom ${PROJECT_SOURCE_DIR}/src/HelloVikingRoom.cpp)

# Add the generated hpp's to the include directories, plus the library directory
target_include_directories(hellovikingroom PUBLIC "${INCLUDE_DIRS}")

# Add which libraries to link
target_link_libraries(hellovikingroom PUBLIC
                      ${EXTRA_LIBS}
                      ${Vulkan_LIBRARIES}
                      glfw
                      )





##### TARGET FOR TESTS #####
# Specify which file will compile to the executable
add_executable(test_array ${PROJECT_SOURCE_DIR}/tests/Array/test_array.cpp)
# Also add the two test library
add_library(array_initialization ${PROJECT_SOURCE_DIR}/tests/Array/initialization.cpp)

# Add the generated hpp's to the include directories, plus the library directory
target_include_directories(test_array PUBLIC "${INCLUDE_DIRS}")
# Add the generated hpp's to the include directories, plus the library directory
target_include_directories(array_initialization PUBLIC "${INCLUDE_DIRS}")

# Add which libraries to link
target_link_libraries(test_array PUBLIC
                      array_initialization
                      )





##### TARGET FOR COMPILING THE SHADERS #####
# Generate the shaders
add_custom_target(
  build_shaders ALL
  DEPENDS ${PROJECT_BINARY_DIR}/vert.spv ${PROJECT_BINARY_DIR}/frag.spv
)
