# # Specify the libraries in this directory
# add_library(VkDebugger Debugger.cpp)
# add_library(VkInstance Instance.cpp)

# # Set the dependencies for these libraries:
# target_include_directories(VkDebugger PUBLIC
#                            "${INCLUDE_DIRS}")
# target_include_directories(VkInstance PUBLIC
#                            "${INCLUDE_DIRS}")

# # Add it to the list of includes & linked libraries
# list(APPEND EXTRA_LIBS VkDebugger)
# list(APPEND EXTRA_LIBS VkInstance)
# Specify the libraries in this directory
add_library(VulkanLib Debugger.cpp Instance.cpp)
# Set the include directories for these libraries:
target_include_directories(VulkanLib PUBLIC
                           "${INCLUDE_DIRS}")
# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanLib)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
