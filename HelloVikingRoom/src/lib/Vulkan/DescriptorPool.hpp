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

#include "Vulkan/Device.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/DescriptorSetLayout.hpp"
#include "Tools/Array.hpp"

namespace HelloVikingRoom::Vulkan {
    /* Forward declaration of the DescriptorPool class. */
    class DescriptorPool;

    /* The DescriptorSetRef class, which references a DescriptorSet allocated in a certain DescriptorPool. */
    class DescriptorSetRef {
    private:
        /* The VkDescriptorSet class that this struct wraps. */
        VkDescriptorSet vk_descriptor_set;

        /* Constructor for the DescriptorSetRef class, which takes the pool where it is bound and the ready-made VkDescriptorSet to wrap. */
        DescriptorSetRef(const DescriptorPool& descriptor_pool, VkDescriptorSet descriptor_set);

        /* Mark the DescriptorPool as friend. */
        friend class DescriptorPool;
    
    public:
        /* Constant reference to the DescriptorPool where this set is allocated. */
        const DescriptorPool& pool;

        /* Copy constructor for the DescriptorSetRef class. */
        DescriptorSetRef(const DescriptorSetRef& other);
        /* Move constructor for the DescriptorSetRef class. */
        DescriptorSetRef(DescriptorSetRef&& other);
        /* Destructor for the DescriptorSetRef class. */
        ~DescriptorSetRef();

        /* Binds this descriptor set to a given (uniform) buffer. */
        void set(const Buffer& buffer);

        /* Explicitly returns the internal VkDescriptorSet object. */
        inline const VkDescriptorSet& descriptor_set() const { return this->vk_descriptor_set; }
        /* Implicitly casts this class to a VkDescriptorSet by returning the internal object. */
        inline operator VkDescriptorSet() const { return this->vk_descriptor_set; }

    };



    /* The DescriptorPool class, which manages memory for the DescriptorSets. */
    class DescriptorPool {
    private:
        /* The internal VkDescriptorPool object that this class wraps. */
        VkDescriptorPool vk_descriptor_pool;
        /* The list of VkDescriptorSet classes allocated by this pool. */
        Tools::Array<VkDescriptorSet> vk_descriptor_sets;

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

        /* Internally allocated a new DescriptorSet with the given layout, and returns a reference for it. */
        DescriptorSetRef get_descriptor(const DescriptorSetLayout& descriptor_set_layout);
        /* Internally allocated N new DescriptorSets with the given layout, and returns a reference for each of those. */
        Tools::Array<DescriptorSetRef> get_descriptor(size_t N, const DescriptorSetLayout& descriptor_set_layout);

        /* Resizes the pool to allow a new number of swapchain images. Note that this invalidates all existing descriptor set references. */
        void resize(uint32_t n_descriptors, uint32_t n_sets);

        /* Expliticly returns the internal VkDescriptorPool object. */
        inline const VkDescriptorPool& descriptor_pool() const { return this->vk_descriptor_pool; }
        /* Impliticly casts this class to a VkDescriptorPool by returning the internal object. */
        inline operator VkDescriptorPool() const { return this->vk_descriptor_pool; }
    };
}

#endif
