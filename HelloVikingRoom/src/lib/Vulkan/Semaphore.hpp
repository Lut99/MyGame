/* SEMAPHORE.hpp
 *   by Lut99
 *
 * Created:
 *   15/01/2021, 14:55:01
 * Last edited:
 *   15/01/2021, 14:55:01
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Semaphore class, which wraps a VkSemaphore object. These
 *   objects are meant to synchronize between command queue's in in them,
 *   and so that's also the function of this class.
**/

#ifndef VULKAN_SEMAPHORE_HPP
#define VULKAN_SEMAPHORE_HPP

#include <vulkan/vulkan.h>

#include "Vulkan/Device.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The Semaphore class, which is used to synchronize in between command queues (or in them). */
    class Semaphore {
    private:
        /* The VkSemphore object that this class wraps. */
        VkSemaphore vk_semaphore;

        /* The create info for the semaphore so that we can quickly re-create it. */
        VkSemaphoreCreateInfo vk_semaphore_info;
    
    public:
        /* The device class to which this semaphore is bound. */
        const Device& device;

        /* Constructor for the Semaphore class, which only takes the device where the semaphore will function on and live. */
        Semaphore(const Device& device);
        /* Copy constructor for the Semaphore class, which is deleted. */
        Semaphore(const Semaphore& other) = delete;
        /* Move constructor for the Semaphore class. */
        Semaphore(Semaphore&& other);
        /* Destructor for the Semaphore class. */
        ~Semaphore();

        /* Resets the semaphore by destroying and then re-creating the internal VkSemaphore object. */
        void reset();

        /* Expliticly returns the internal VkSemaphore object. */
        inline VkSemaphore semaphore() const { return this->vk_semaphore; }
        /* Implicitly casts this class to a VkSemaphore by returning the internal object. */
        inline operator VkSemaphore() const { return this->vk_semaphore; }

    };
};

#endif
