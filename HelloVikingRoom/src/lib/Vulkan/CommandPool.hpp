/* COMMAND POOL.hpp
 *   by Lut99
 *
 * Created:
 *   14/01/2021, 17:01:08
 * Last edited:
 *   14/01/2021, 17:01:08
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The CommandPool class is a simple wrapper around the Vulkan
 *   VkCommandPool class, which is used to easily manage & register command
 *   buffers.
**/

#ifndef COMMAND_POOL_HPP
#define COMMAND_POOL_HPP

#include <vulkan/vulkan.h>

#include "Vulkan/Device.hpp"
#include "Tools/Array.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The CommandPool class manages memory used for a certain class of command buffers. */
    class CommandPool;

    /* The CommandBuffer class wraps around a standard command buffer, which makes handling them a little easier. */
    class CommandBuffer {
    private:
        /* The VkCommandBuffer object that this class wraps. */
        VkCommandBuffer vk_command_buffer;

        /* Constructor for the CommandBuffer class, which takes the command pool to create it in and the VkCommandBuffer to wrap. */
        CommandBuffer(const CommandPool& command_pool, VkCommandBuffer command_buffer);

        /* Declare the CommandPool as friend. */
        friend class CommandPool;
    
    public:
        /* The CommandPool class where this buffer was created. */
        const CommandPool& command_pool;

        /* Copy constructor for the CommandBuffer class, which is deleted. */
        CommandBuffer(const CommandBuffer& other) = delete;
        /* Move constructor for the CommandBuffer class. */
        CommandBuffer(CommandBuffer&& other);
        /* Destructor for the CommandBuffer class. */
        ~CommandBuffer();

        /* Begins recording the command buffer. Optionally takes any flags to start the command buffer with. */
        void begin(VkCommandBufferUsageFlags flags = 0);
        /* Stops recording the command buffer. */
        void end();

        /* Expliticly returns the internal VkCommandBuffer object. */
        inline const VkCommandBuffer& command_buffer() const { return this->vk_command_buffer; }
        /* Implicitly casts this class to a VkCommandBuffer by returning the internal object. */
        inline operator VkCommandBuffer() const { return this->vk_command_buffer; }

    };



    /* The CommandPool class manages memory used for a certain class of command buffers. */
    class CommandPool {
    private:
        /* The VKCommandPool object that is wrapped by this class. */
        VkCommandPool vk_command_pool;

    public:
        /* The device to which this pool is bound. */
        const Device& device;
        /* The queue family for which this pool manages the memory. */
        const uint32_t queue_family;

        /* Constructor for the CommandPool class, which takes a device to schedule commands on, the queue index for which this command pool is responsible and optionally flags to use during creation. */
        CommandPool(const Device& device, uint32_t queue_family, VkCommandPoolCreateFlags flags = 0);
        /* Copy constructor for the CommandPool class, which is deleted. */
        CommandPool(const CommandPool& other) = delete;
        /* Move constructor for the CommandPool class. */
        CommandPool(CommandPool&& other);
        /* Destructor for the CommandPool class. */
        ~CommandPool();

        /* Returns a new command buffer at the given level. */
        CommandBuffer get_buffer(VkCommandBufferLevel buffer_level);
        // /* Returns N new command buffers at the given level. */
        // Tools::Array<CommandBuffer> get_buffer(uint32_t N, VkCommandBufferLevel buffer_level);

        /* Explicitly returns the internal VkCommandPool object. */
        inline VkCommandPool command_pool() const { return this->vk_command_pool; }
        /* Implicitly casts this class to a VkCommandPool, by returning the internal object. */
        inline operator VkCommandPool() const { return this->vk_command_pool; }

    };
}

#endif
