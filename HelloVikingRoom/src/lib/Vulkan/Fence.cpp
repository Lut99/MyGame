/* FENCE.cpp
 *   by Lut99
 *
 * Created:
 *   15/01/2021, 14:55:36
 * Last edited:
 *   15/01/2021, 14:55:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Fence class, which wraps a VkFence object. These objects
 *   are used to synchronize the rendering process with our application,
 *   and thus that's also what the Fence class will do.
**/

#include "Debug/Debug.hpp"
#include "Fence.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** FENCE CLASS *****/
/* Constructor for the Fence class, which only takes the Device where the Fence functions and lives. */
Fence::Fence(const Device& device) :
    device(device)
{
    DENTER("Vulkan::Fence::Fence");
    DLOG(info, "Intializing Vulkan fence...");

    // Create the creation struct for all fences
    VkFenceCreateInfo fence_info;
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    // Tell it to start in a signalled state
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fence_info.pNext = nullptr;

    // Actually create the fence
    if (vkCreateFence(this->device, &fence_info, nullptr, &this->vk_fence) != VK_SUCCESS) {
        DLOG(fatal, "Could not create fence object.");
    }

    DLEAVE;
}

/* Move constructor for the Fence class. */
Fence::Fence(Fence&& other) :
    vk_fence(other.vk_fence),
    device(device)
{
    other.vk_fence = nullptr;
}

/* Destructor for the Fence class. */
Fence::~Fence() {
    DENTER("Vulkan::Fence::~Fence");
    DLOG(info, "Cleaning Vulkan fence...");

    if (this->vk_fence != nullptr) {
        vkDestroyFence(this->device, this->vk_fence, nullptr);
    }

    DLEAVE;
}



/* Waits for this fence to be reached. */
void Fence::wait() {
    DENTER("Vulkan::Fence::wait");

    // Simply call the vkWaitForFences function, which does exactly this
    vkWaitForFences(this->device, 1, &this->vk_fence, VK_TRUE, UINT64_MAX);

    DRETURN;
}

/* Resets this fence to its initial state. */
void Fence::reset() {
    DENTER("Vulkan::Fence::reset");

    // Simply call the vkResetFences function, which does exactly this
    vkResetFences(this->device, 1, &this->vk_fence);

    DRETURN;
}
