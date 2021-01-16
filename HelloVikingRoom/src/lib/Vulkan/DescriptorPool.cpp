/* DESCRIPTOR POOL.cpp
 *   by Lut99
 *
 * Created:
 *   16/01/2021, 12:50:09
 * Last edited:
 *   16/01/2021, 12:50:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorPool class, which wraps VkDescriptorPool
 *   objects which manage descriptor memory.
**/

#include "Debug/Debug.hpp"
#include "DescriptorPool.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** DESCRIPTORPOOL CLASS *****/
/* Constructor for the DescriptorPool class, which takes the device to create the pool on, the number of descriptors we want to allocate in the pool, the maximum number of descriptor sets that can be allocated and optionally create flags. */
DescriptorPool::DescriptorPool(const Device& device, uint32_t n_descriptors, uint32_t n_sets, VkDescriptorPoolCreateFlags flags) :
    vk_descriptor_pool(nullptr),
    device(device),
    vk_descriptor_pool_size({}),
    vk_descriptor_pool_info({})
{
    DENTER("Vulkan::DescriptorPool::DescriptorPool");
    DLOG(info, "Creating Vulkan descriptor pool...");

    // Start by filling in the descriptor size struct for a uniform buffer
    this->vk_descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    // Continue by filling in the create info
    this->vk_descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // Set the pool size to use
    this->vk_descriptor_pool_info.poolSizeCount = 1;
    this->vk_descriptor_pool_info.pPoolSizes = &this->vk_descriptor_pool_size;
    // Set the flags
    this->vk_descriptor_pool_info.flags = flags;

    // Now, use the resize() function to do the generation itself
    this->resize(n_descriptors, n_sets);

    DLEAVE;
}

/* Move constructor for the DescriptorPool class. */
DescriptorPool::DescriptorPool(DescriptorPool&& other) :
    vk_descriptor_pool(other.vk_descriptor_pool),
    device(other.device),
    vk_descriptor_pool_size(other.vk_descriptor_pool_size),
    vk_descriptor_pool_info(other.vk_descriptor_pool_info)
{
    other.vk_descriptor_pool = nullptr;
}

/* Destructor for the DescriptorPool class. */
DescriptorPool::~DescriptorPool() {
    DENTER("Vulkan::DescriptorPool::~DescriptorPool");
    DLOG(info, "Cleaning Vulkan descriptor pool...");

    if (this->vk_descriptor_pool != nullptr) {
        vkDestroyDescriptorPool(this->device, this->vk_descriptor_pool, nullptr);
    }

    DLEAVE;
}



/* Resizes the pool to allow a new number of swapchain images. */
void DescriptorPool::resize(uint32_t n_descriptors, uint32_t n_sets) {
    DENTER("Vulkan::DescriptorPool::resize");

    // Destroy any existing pool
    if (this->vk_descriptor_pool != nullptr) {
        vkDestroyDescriptorPool(this->device, this->vk_descriptor_pool, nullptr);
    }

    // Update the structs with the new sizes
    this->vk_descriptor_pool_size.descriptorCount = n_descriptors;
    this->vk_descriptor_pool_info.maxSets = n_sets;

    // Generate the pool
    if (vkCreateDescriptorPool(this->device, &this->vk_descriptor_pool_info, nullptr, &this->vk_descriptor_pool) != VK_SUCCESS) {
        DLOG(fatal, "Could not create descriptor pool.");
    }

    // Done
    DRETURN;
}
