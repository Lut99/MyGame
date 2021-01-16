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


/***** DESCRIPTORSETREF CLASS *****/
/* Constructor for the DescriptorSetRef class, which takes the pool where it is bound and the ready-made VkDescriptorSet to wrap. */
DescriptorSetRef::DescriptorSetRef(const DescriptorPool& descriptor_pool, VkDescriptorSet descriptor_set) :
    vk_descriptor_set(descriptor_set),
    pool(descriptor_pool)
{}

/* Copy constructor for the DescriptorSetRef class. */
DescriptorSetRef::DescriptorSetRef(const DescriptorSetRef& other) :
    vk_descriptor_set(other.vk_descriptor_set),
    pool(other.pool)
{}

/* Move constructor for the DescriptorSetRef class. */
DescriptorSetRef::DescriptorSetRef(DescriptorSetRef&& other) :
    vk_descriptor_set(std::move(other.vk_descriptor_set)),
    pool(std::move(other.pool))
{}

/* Destructor for the DescriptorSetRef class. */
DescriptorSetRef::~DescriptorSetRef() {}



/* Binds this descriptor set to a given (uniform) buffer. */
void DescriptorSetRef::set(const Buffer& buffer) {
    DENTER("Vulkan::DescriptorSetRef::set");

    // Start by creating the buffer info s.t. the descriptor knows what is bound
    VkDescriptorBufferInfo buffer_info{};
    // Tell it which buffer to bind
    buffer_info.buffer = buffer;
    // Tell it the offset in the buffer
    buffer_info.offset = buffer.offset();
    // Tell it which part of the buffer to copy; will likely be everything
    buffer_info.range = buffer.size();

    // Update the descriptor using an VkWriteDescriptorSet. Can also be done using a VkCopyDescriptorSet to copy from one descriptor to another.
    VkWriteDescriptorSet write_info{};
    write_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    // Set the set to which we write
    write_info.dstSet = this->vk_descriptor_set;
    // Set the binding to use (equal to the one in the shader)
    write_info.dstBinding = 0;
    // Set the element in the array; since we don't use that, it's a 0
    write_info.dstArrayElement = 0;
    // Specify how many sets and which type it has
    write_info.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_info.descriptorCount = 1;
    // The data to pass. Can be of multiple types, but we want to update a buffer so we use that
    write_info.pBufferInfo = &buffer_info;
    write_info.pImageInfo = nullptr;
    write_info.pTexelBufferView = nullptr;

    // Actually perform the update! Note that we can pass both multiple writes and copies at the same time, but we only pass writes
    vkUpdateDescriptorSets(this->pool.device, 1, &write_info, 0, nullptr);

    // We're done
    DRETURN;
}





/***** DESCRIPTORPOOL CLASS *****/
/* Constructor for the DescriptorPool class, which takes the device to create the pool on, the number of descriptors we want to allocate in the pool, the maximum number of descriptor sets that can be allocated and optionally create flags. */
DescriptorPool::DescriptorPool(const Device& device, uint32_t n_descriptors, uint32_t n_sets, VkDescriptorPoolCreateFlags flags) :
    vk_descriptor_pool(nullptr),
    vk_descriptor_sets(n_sets),
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
    this->vk_descriptor_pool_info.flags = flags | VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    // Now, use the resize() function to do the generation itself
    this->resize(n_descriptors, n_sets);

    DLEAVE;
}

/* Move constructor for the DescriptorPool class. */
DescriptorPool::DescriptorPool(DescriptorPool&& other) :
    vk_descriptor_pool(std::move(other.vk_descriptor_pool)),
    vk_descriptor_sets(std::move(other.vk_descriptor_sets)),
    device(other.device),
    vk_descriptor_pool_size(other.vk_descriptor_pool_size),
    vk_descriptor_pool_info(other.vk_descriptor_pool_info)
{
    other.vk_descriptor_pool = nullptr;
    other.vk_descriptor_sets.clear();
}

/* Destructor for the DescriptorPool class. */
DescriptorPool::~DescriptorPool() {
    DENTER("Vulkan::DescriptorPool::~DescriptorPool");
    DLOG(info, "Cleaning Vulkan descriptor pool...");

    // Destroy any existing sets
    if (this->vk_descriptor_sets.size() > 0) {
        if (vkFreeDescriptorSets(this->device, this->vk_descriptor_pool, static_cast<uint32_t>(this->vk_descriptor_sets.size()), this->vk_descriptor_sets.rdata()) != VK_SUCCESS) {
            DLOG(nonfatal, "Failed to deallocate descriptor sets.");
        }
    }
    // Destroy any existing pool
    if (this->vk_descriptor_pool != nullptr) {
        vkDestroyDescriptorPool(this->device, this->vk_descriptor_pool, nullptr);
    }

    DLEAVE;
}



/* Internally allocated a new DescriptorSet with the given layout, and returns a reference for it. */
DescriptorSetRef DescriptorPool::get_descriptor(const DescriptorSetLayout& descriptor_set_layout) {
    DENTER("Vulkan::DescriptorPool::get_descriptor");

    // Check if we have enough space left
    if (static_cast<uint32_t>(this->vk_descriptor_sets.size()) >= this->vk_descriptor_pool_info.maxSets) {
        DLOG(fatal, "Cannot allocate more than " + std::to_string(this->vk_descriptor_pool_info.maxSets) + " descriptor sets in this pool.");
    }

    // Start by creating the allocate info
    VkDescriptorSetAllocateInfo descriptor_set_info{};
    descriptor_set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    // Set the pool that is used to allocate them
    descriptor_set_info.descriptorPool = this->vk_descriptor_pool;
    // Set the number of sets to allocate (1, in this case)
    descriptor_set_info.descriptorSetCount = 1;
    // Set the layout for this descriptor
    descriptor_set_info.pSetLayouts = &descriptor_set_layout.descriptor_set_layout();

    // Time to call the allocate
    if (vkAllocateDescriptorSets(this->device, &descriptor_set_info, this->vk_descriptor_sets.wdata(this->vk_descriptor_sets.size() + 1)) != VK_SUCCESS) {
        DLOG(fatal, "Failed to allocate a new descriptor set.");
    }

    // Wraps it in a reference and we're done
    DRETURN DescriptorSetRef(*this, this->vk_descriptor_sets[this->vk_descriptor_sets.size() - 1]);
}

/* Internally allocated N new DescriptorSets with the given layout, and returns a reference for each of those. */
Tools::Array<DescriptorSetRef> DescriptorPool::get_descriptor(size_t N, const DescriptorSetLayout& descriptor_set_layout) {
    DENTER("Vulkan::DescriptorPool::get_descriptor(multiple)");

    // Check if we have enough space to ever accept the amount
    if (static_cast<uint32_t>(N) > this->vk_descriptor_pool_info.maxSets) {
        DLOG(fatal, "Cannot allocate " + std::to_string(N) + " descriptor sets in this pool with a maximum of " + std::to_string(this->vk_descriptor_pool_info.maxSets) + " sets.");
    }
    // Check if we have enough space left
    if (static_cast<uint32_t>(this->vk_descriptor_sets.size()) + static_cast<uint32_t>(N) > this->vk_descriptor_pool_info.maxSets) {
        DLOG(fatal, "Cannot allocate more than " + std::to_string(this->vk_descriptor_pool_info.maxSets) + " descriptor sets in this pool.");
    }

    // Copy the layout N times
    Array<VkDescriptorSetLayout> layouts(N);
    for (size_t i = 0; i < N; i++) {
        layouts.push_back(descriptor_set_layout);
    }

    // Start by creating the allocate info
    VkDescriptorSetAllocateInfo descriptor_set_info{};
    descriptor_set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    // Set the pool that is used to allocate them
    descriptor_set_info.descriptorPool = this->vk_descriptor_pool;
    // Set the number of sets to allocate (1, in this case)
    descriptor_set_info.descriptorSetCount = static_cast<uint32_t>(layouts.size());
    // Set the layout for this descriptor
    descriptor_set_info.pSetLayouts = layouts.rdata();

    // Time to call the allocate
    if (vkAllocateDescriptorSets(this->device, &descriptor_set_info, this->vk_descriptor_sets.wdata(this->vk_descriptor_sets.size() + N)) != VK_SUCCESS) {
        DLOG(fatal, "Failed to allocate " + std::to_string(N) + " new descriptor sets.");
    }

    // Wrap them in an Array of references and we're done
    Array<DescriptorSetRef> result(N);
    for (size_t i = 1; i <= N; i++) {
        result.push_back(DescriptorSetRef(
            *this, this->vk_descriptor_sets[this->vk_descriptor_sets.size() - i]
        ));
    }
    DRETURN result;
}



/* Resizes the pool to allow a new number of swapchain images. */
void DescriptorPool::resize(uint32_t n_descriptors, uint32_t n_sets) {
    DENTER("Vulkan::DescriptorPool::resize");

    // Destroy any existing sets
    if (this->vk_descriptor_sets.size() > 0) {
        if (vkFreeDescriptorSets(this->device, this->vk_descriptor_pool, static_cast<uint32_t>(this->vk_descriptor_sets.size()), this->vk_descriptor_sets.rdata()) != VK_SUCCESS) {
            DLOG(nonfatal, "Failed to deallocate descriptor sets.");
        }
        this->vk_descriptor_sets.clear();
    }
    // Destroy any existing pool
    if (this->vk_descriptor_pool != nullptr) {
        vkDestroyDescriptorPool(this->device, this->vk_descriptor_pool, nullptr);
    }

    // Update the structs with the new sizes
    this->vk_descriptor_pool_size.descriptorCount = n_descriptors;
    this->vk_descriptor_pool_info.maxSets = n_sets;

    // Update our internal size and the list of new sizes
    this->vk_descriptor_sets.reserve(this->vk_descriptor_pool_info.maxSets);

    // Generate the pool
    if (vkCreateDescriptorPool(this->device, &this->vk_descriptor_pool_info, nullptr, &this->vk_descriptor_pool) != VK_SUCCESS) {
        DLOG(fatal, "Could not create descriptor pool.");
    }

    // Done
    DRETURN;
}
