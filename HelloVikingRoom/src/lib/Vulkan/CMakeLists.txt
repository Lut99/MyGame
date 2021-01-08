# Specify the libraries in this directory
add_library(VulkanLib Debugger.cpp Instance.cpp Device.cpp Swapchain.cpp)
# Set the include directories for these libraries:
target_include_directories(VulkanLib PUBLIC
                           "${INCLUDE_DIRS}")
# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanLib)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
