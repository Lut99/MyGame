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

namespace HelloVikingRoom::Vulkan {
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

        /* Explicitly returns the internal VkCommandPool object. */
        inline VkCommandPool command_pool() const { return this->vk_command_pool; }
        /* Implicitly casts this class to a VkCommandPool, by returning the internal object. */
        inline operator VkCommandPool() const { return this->vk_command_pool; }

    };
}

#endif
