/* FENCE.hpp
 *   by Lut99
 *
 * Created:
 *   15/01/2021, 14:55:42
 * Last edited:
 *   15/01/2021, 14:55:42
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Fence class, which wraps a VkFence object. These objects
 *   are used to synchronize the rendering process with our application,
 *   and thus that's also what the Fence class will do.
**/

#ifndef VULKAN_FENCE_HPP
#define VULKAN_FENCE_HPP

#include <vulkan/vulkan.h>

#include "Vulkan/Device.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The Fence class is used for synchronizing the render process with the rest of the application. */
    class Fence {
    private:
        /* The internal VkFence object that this class wraps. */
        VkFence vk_fence;

    public:
        /* The Device class that this Fence functions on and lives on. */
        const Device& device;

        /* Constructor for the Fence class, which only takes the Device where the Fence functions and lives. */
        Fence(const Device& device);
        /* Copy constructor for the Fence class, which is deleted. */
        Fence(const Fence& other) = delete;
        /* Move constructor for the Fence class. */
        Fence(Fence&& other);
        /* Destructor for the Fence class. */
        ~Fence();

        /* Waits for this fence to be reached. */
        void wait();
        /* Resets this fence to its initial state. */
        void reset();

        /* Expliticly returns the internal VkFence object. */
        inline const VkFence& fence() const { return this->vk_fence; }
        /* Implicitly casts this class to a VkFence by returning the internal object. */
        inline operator VkFence() const { return this->vk_fence; }

    };
}

#endif
