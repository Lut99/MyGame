cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(HelloVikingRoom VERSION 1.0.0)

# Be sure that both Vulkan & GLFW are installed
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define a .hpp file we can use to generate some C++-side interesting constants
configure_file(HelloVikingRoom.hpp.in HelloVikingRoom.hpp)

# Define all include directories
get_target_property(GLFW_DIR glfw INTERFACE_INCLUDE_DIRECTORIES)
SET(INCLUDE_DIRS "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/src/lib" "${Vulkan_INCLUDE_DIRS}" "${GLFW_DIR}")

# Add the lib subdirectory
add_subdirectory(src)

# Specify which file will compile to the executable
add_executable(HelloVikingRoom ${PROJECT_SOURCE_DIR}/src/HelloVikingRoom.cpp ${EXTRA_HEADERS})

message(STATUS "${INCLUDE_DIRS}")
message(STATUS "${EXTRA_LIBS}")

# Add the generated hpp's to the include directories, plus the library directory
include_directories("${PROJECT_BINARY_DIR}"
                    "${PROJECT_SOURCE_DIR}/src/lib"
                    "${Vulkan_INCLUDE_DIRS}")

# Add which libraries to link
target_link_libraries(HelloVikingRoom PUBLIC
                      ${EXTRA_LIBS}
                      ${Vulkan_LIBRARIES}
                      glfw
                      )

# Generate the shaders
add_custom_target(
  BuildShaders ALL
  DEPENDS ${PROJECT_BINARY_DIR}/vert.spv ${PROJECT_BINARY_DIR}/frag.spv
)
