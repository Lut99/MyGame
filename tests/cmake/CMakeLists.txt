cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(CmakeTest VERSION 1.0.0)

# Be sure that both Vulkan & GLFW are installed
find_package(Vulkan REQUIRED)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify which file will compile to the executable
add_executable(CmakeTest ${PROJECT_SOURCE_DIR}/test.cpp)

# Add the generated hpp's to the include directories, plus the library directory
target_include_directories(CmakeTest PUBLIC
                           ${Vulkan_INCLUDE_DIRS}
                           )

# Add which libraries to link
target_link_libraries(CmakeTest PUBLIC
                      ${Vulkan_LIBRARIES}
                      )
