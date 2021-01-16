# Specify the libraries in this directory
add_library(VulkanLib Debugger.cpp Instance.cpp Device.cpp Swapchain.cpp RenderPass.cpp ShaderModule.cpp GraphicsPipeline.cpp Framebuffer.cpp CommandPool.cpp Buffer.cpp Semaphore.cpp Fence.cpp DescriptorSetLayout.cpp DescriptorPool.cpp)
# Set the include directories for these libraries:
target_include_directories(VulkanLib PUBLIC
                           "${INCLUDE_DIRS}")
# Add it to the list of includes & linked libraries
list(APPEND EXTRA_LIBS VulkanLib)

# Add the subdirectories before returning
add_subdirectory(GraphicsPipelines)
add_subdirectory(RenderPasses)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
