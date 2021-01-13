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

# Load the libraries
add_subdirectory(src)



##### HELLOVIKINGROOM TARGET #####
# Specify which file will compile to the executable
add_executable(hellovikingroom ${PROJECT_SOURCE_DIR}/src/HelloVikingRoom.cpp)
# Set the output to bin directory
set_target_properties(hellovikingroom
                      PROPERTIES 
                      RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin
                      )

# Add the generated hpp's to the include directories, plus the library directory
target_include_directories(hellovikingroom PUBLIC "${INCLUDE_DIRS}")

# Add which libraries to link
target_link_libraries(hellovikingroom PUBLIC
                      ${EXTRA_LIBS}
                      ${Vulkan_LIBRARIES}
                      glfw
                      )



##### BUILDING SHADERS #####
# Define the custom commands to compile the shaders
add_custom_command(TARGET hellovikingroom POST_BUILD
    COMMAND glslc -o ${PROJECT_SOURCE_DIR}/bin/vert.spv ${PROJECT_SOURCE_DIR}/src/lib/Shaders/shader.vert
    COMMAND glslc -o ${PROJECT_SOURCE_DIR}/bin/frag.spv ${PROJECT_SOURCE_DIR}/src/lib/Shaders/shader.frag
    COMMENT "Building shaders..."
)



##### TARGET FOR TESTS #####
# Specify which file will compile to the executable
add_executable(test_array ${PROJECT_SOURCE_DIR}/tests/Array/test_array.cpp)
# Also add the test libraries
add_library(array_initialization ${PROJECT_SOURCE_DIR}/tests/Array/initialization.cpp)
add_library(array_consistancy ${PROJECT_SOURCE_DIR}/tests/Array/consistancy.cpp)
add_library(array_resize ${PROJECT_SOURCE_DIR}/tests/Array/resize.cpp)
add_library(array_push_erase ${PROJECT_SOURCE_DIR}/tests/Array/push_erase.cpp)

# Add the generated hpp's to the include directories, plus the library directory
target_include_directories(test_array PUBLIC "${INCLUDE_DIRS}")
# Add the generated hpp's to the include directories, plus the library directory
target_include_directories(array_initialization PUBLIC "${INCLUDE_DIRS}")
target_include_directories(array_consistancy PUBLIC "${INCLUDE_DIRS}")
target_include_directories(array_resize PUBLIC "${INCLUDE_DIRS}")
target_include_directories(array_push_erase PUBLIC "${INCLUDE_DIRS}")

# Add which libraries to link
target_link_libraries(test_array PUBLIC
                      array_initialization
                      array_consistancy
                      array_resize
                      array_push_erase
                      )
