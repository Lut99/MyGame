/* COMMAND POOL.cpp
 *   by Lut99
 *
 * Created:
 *   14/01/2021, 17:01:05
 * Last edited:
 *   14/01/2021, 17:01:05
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The CommandPool class is a simple wrapper around the Vulkan
 *   VkCommandPool class, which is used to easily manage & register command
 *   buffers.
**/

#include "Debug/Debug.hpp"
#include "CommandPool.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** COMMANDPOOL CLASS *****/
/* Constructor for the CommandPool class, which takes a device to schedule commands on, the queue index for which this command pool is responsible and optionally flags to use during creation. */
CommandPool::CommandPool(const Device& device, uint32_t queue_family, VkCommandPoolCreateFlags flags) :
    device(device),
    queue_family(queue_family)
{
    DENTER("Vulkan::CommandPool::CommandPool");
    DLOG(info, "Creating Vulkan command pool for queue " + std::to_string(queue_family) + "...");

    // Start by creating the obvious create_info
    VkCommandPoolCreateInfo command_pool_info{};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // Set the queue for this pool
    command_pool_info.queueFamilyIndex = queue_family;
    // Optionally provide some flags
    command_pool_info.flags = flags;

    // Create it
    if (vkCreateCommandPool(this->device, &command_pool_info, nullptr, &this->vk_command_pool) != VK_SUCCESS) {
        DLOG(fatal, "Could not create command pool for queue " + std::to_string(queue_family));
    }

    DLEAVE;
}

/* Move constructor for the CommandPool class. */
CommandPool::CommandPool(CommandPool&& other) :
    vk_command_pool(other.vk_command_pool),
    device(other.device),
    queue_family(other.queue_family)
{
    other.vk_command_pool = nullptr;
}

/* Destructor for the CommandPool class. */
CommandPool::~CommandPool() {
    DENTER("Vulkan::CommandPool::~CommandPool");
    DLOG(info, "Cleaning Vulkan command pool for queue " + std::to_string(queue_family) + "...");

    if (this->vk_command_pool != nullptr) {
        vkDestroyCommandPool(this->device, this->vk_command_pool, nullptr);
    }

    DLEAVE;
}
