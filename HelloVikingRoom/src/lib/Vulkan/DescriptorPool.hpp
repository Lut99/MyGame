/* DESCRIPTOR POOL.hpp
 *   by Lut99
 *
 * Created:
 *   16/01/2021, 12:50:01
 * Last edited:
 *   16/01/2021, 12:50:01
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorPool class, which wraps VkDescriptorPool
 *   objects which manage descriptor memory.
**/

#ifndef VULKAN_DESCRIPTOR_POOL_HPP
#define VULKAN_DESCRIPTOR_POOL_HPP

#include <vulkan/vulkan.h>

#include "Device.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The DescriptorPool class, which manages memory for the DescriptorSets. */
    class DescriptorPool {
    private:
        /* The internal VkDescriptorPool object that this class wraps. */
        VkDescriptorPool vk_descriptor_pool;

    public:
        /* Constant reference to the device that his pool is bound to. */
        const Device& device;
        /* The VkDescriptorPoolSize struct that defines how many descriptors the pool can allocate. */
        VkDescriptorPoolSize vk_descriptor_pool_size;
        /* The VkDescriptorPooLCreateInfo used to quickly resize the pool to a new amount of descriptors. */
        VkDescriptorPoolCreateInfo vk_descriptor_pool_info;

        /* Constructor for the DescriptorPool class, which takes the device to create the pool on, the number of descriptors we want to allocate in the pool, the maximum number of descriptor sets that can be allocated and optionally create flags. */
        DescriptorPool(const Device& device, uint32_t n_descriptors, uint32_t n_sets, VkDescriptorPoolCreateFlags flags = 0);
        /* Copy constructor for the DescriptorPool class, which is deleted. */
        DescriptorPool(const DescriptorPool& other) = delete;
        /* Move constructor for the DescriptorPool class. */
        DescriptorPool(DescriptorPool&& other);
        /* Destructor for the DescriptorPool class. */
        ~DescriptorPool();

        /* Resizes the pool to allow a new number of swapchain images. */
        void resize(uint32_t n_descriptors, uint32_t n_sets);

        /* Expliticly returns the internal VkDescriptorPool object. */
        inline const VkDescriptorPool& descriptor_pool() const { return this->vk_descriptor_pool; }
        /* Impliticly casts this class to a VkDescriptorPool by returning the internal object. */
        inline operator VkDescriptorPool() const { return this->vk_descriptor_pool; }
    };
}

#endif
