/* BUFFER.cpp
 *   by Lut99
 *
 * Created:
 *   14/01/2021, 15:55:44
 * Last edited:
 *   14/01/2021, 15:55:44
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The buffer class is used to manage generic Vulkan buffers. Basically
 *   takes care of any memory management using standard C++ class
 *   functions.
 * 
 *   For now, we'll use individual VkAllocate-calls for each buffer, but for
 *   more serious projects, this should be changed to use an allocator that
 *   pools the allocates.
**/

#include "Debug/Debug.hpp"
#include "Buffer.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** BUFFER CLASS *****/
/* Constructor for the Buffer class, which takes the Device to bind the buffer to, the size of the buffer (in bytes), flags describing how the buffer will be used (for optimizations), flags that mark the requirements we have for the buffer and its memory and optionally create flags for the buffer. */
Buffer::Buffer(const Device& device, VkDeviceSize n_bytes, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags buffer_requirements, VkBufferCreateFlags buffer_flags) :
    vk_mem_property_flags(buffer_requirements),
    vk_buffer_size(n_bytes),
    vk_usage_flags(buffer_usage),
    device(device)
{
    DENTER("Vulkan::Buffer::Buffer");
    DLOG(info, "Creating Vulkan buffer...");

    // Prepare the struct to create the buffer
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    // Tell the program the size of the buffer
    buffer_info.size = this->vk_buffer_size;
    // Tell it how we'll use the buffer
    buffer_info.usage = this->vk_usage_flags;
    // Tell the create info that only one queue needs to use this buffer at a time
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // Pass any flags
    buffer_info.flags = buffer_flags;

    // Create the buffer object
    if (vkCreateBuffer(this->device, &buffer_info, nullptr, &this->vk_buffer) != VK_SUCCESS) {
        DLOG(fatal, "Could not create buffer object.");
    }



    // Next, prepare to allocate the memory
    DLOG(auxillary, "Allocating memory...");
    // First check which requirements our buffer has given itself
    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(this->device, this->vk_buffer, &mem_requirements);

    // Prepare the create info
    VkMemoryAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // Tell it how many bytes to allocate (may differ from n_bytes)
    allocate_info.allocationSize = mem_requirements.size;
    // Use the static helper to find the correct memory type, and allocate on that
    allocate_info.memoryTypeIndex = Buffer::get_memory_type(this->device, mem_requirements.memoryTypeBits, buffer_requirements);

    // Use that struct to allocate the memory
    if (vkAllocateMemory(this->device, &allocate_info, nullptr, &this->vk_memory) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate memory for buffer.");
    }

    // With the memory allocated, bind it to the buffer
    // Note that we'll use an offset for 0, since we're not sharing allocations
    if (vkBindBufferMemory(this->device, this->vk_buffer, this->vk_memory, 0) != VK_SUCCESS) {
        DLOG(fatal, "Could not bind memory to buffer.");
    }

    DLEAVE;
}

/* Move constructor for the Buffer class. */
Buffer::Buffer(Buffer&& other) :
    vk_memory(other.vk_memory),
    vk_mem_property_flags(other.vk_mem_property_flags),
    vk_buffer(other.vk_buffer),
    vk_buffer_size(other.vk_buffer_size),
    vk_usage_flags(other.vk_usage_flags),
    device(other.device)
{
    other.vk_memory = nullptr;
    other.vk_buffer = nullptr;
}

/* Destructor for the Buffer class. */
Buffer::~Buffer() {
    DENTER("Vulkan::Buffer::~Buffer");
    DLOG(info, "Cleaning Vulkan buffer...");

    if (this->vk_buffer != nullptr) {
        vkDestroyBuffer(this->device, this->vk_buffer, nullptr);
    }
    if (this->vk_memory != nullptr) {
        vkFreeMemory(this->device, this->vk_memory, nullptr);
    }

    DLEAVE;
}



/* Returns the appropriate memory type based on the given requirements as stated by a VkBuffer and custom requirements. */
uint32_t Buffer::get_memory_type(const Device& device, uint32_t type_filter, VkMemoryPropertyFlags properties) {
    DENTER("Vulkan::Buffer::get_memory_type");

    // Get the available memory in the internal device
    VkPhysicalDeviceMemoryProperties memory_properties;
    vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);

    // Try to find suitable memory (i.e., check if the device has enough memory bits(?) and if the required properties match)
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
        if (type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            DRETURN i;
        }
    }

    // Didn't find any
    DLOG(fatal, "Could not find suitable memory for buffer on selected device.");
    DRETURN 0;
}

/* Copies one Buffer to the other. Both buffers must be on the same device, and the source buffer must have VK_BUFFER_USAGE_TRANSFER_SRC_BIT and the destination buffer VK_BUFFER_TRANSFER_DST_BIT. Additionally, the destination buffer must have at least as many bytes allocated as the source one. The final argument is the command pool where we'll create the temporary command buffer for the copy. */
void Buffer::copy(Buffer& destination, const Buffer& source, CommandPool& command_pool) {
    DENTER("Vulkan::Buffer::copy");

    // First, check if the device, bits and size matches
    if (&destination.device != &source.device) {
        DLOG(fatal, "Destination and source buffers do not live on the same device.");
    }
    if (destination.size() < source.size()) {
        DLOG(fatal, "Destination buffer is not large enough to receive the source buffer.");
    }
    // if (!(destination.usage() & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
    //     DLOG(fatal, "Destination buffer does not have the VK_BUFFER_USAGE_TRANSFER_DST_BIT set.");
    // }
    // if (!(source.usage() & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
    //     DLOG(fatal, "Destination buffer does not have the VK_BUFFER_USAGE_TRANSFER_SRC_BIT set.");
    // }

    // Get a temporary command buffer
    CommandBuffer command_buffer = command_pool.get_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // Schedule the copy
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = source.size();
    vkCmdCopyBuffer(command_buffer, source, destination, 1, &copy_region);

    // Since that's all we wanna do, we're done here
    command_buffer.end(destination.device.graphics_queue());

    DLEAVE;
}

/* Populates the buffer directly, by mapping the appropriate device memory to host memory and then copying the data in the given array. Note that to do this, this array must have VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set. */
void Buffer::set(void* data, size_t n_bytes) {
    DENTER("Vulkan::Buffer::set");

    // Check if the correct bit is set
    if (!(this->vk_mem_property_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        DLOG(fatal, "Tried to populate buffer that is not visible by the host.");
    }
    // Check if the target buffer has enough memory
    if (this->vk_buffer_size < (VkDeviceSize) n_bytes) {
        DLOG(fatal, "Not enough memory in buffer to accept data of " + std::to_string(n_bytes) + " bytes (buffer has " + std::to_string(this->vk_buffer_size) + " bytes).");
    }

    // If so, then set map the device memory in the buffer to host memory
    void* mapped_memory;
    if (vkMapMemory(this->device, this->vk_memory, 0, (VkDeviceSize) n_bytes, 0, &mapped_memory) != VK_SUCCESS) {
        DLOG(fatal, "Could not map buffer memory to host memory.");
    }

    // Once mapped, copy the memory over
    memcpy(mapped_memory, data, n_bytes);

    // Next, flush the memory to the device if the cache is not coherent
    if (!(this->vk_mem_property_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        VkMappedMemoryRange memory_range{};
        memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        // Set the mapped memory to flush
        memory_range.memory = this->vk_memory;
        // Set the offset for this memory (always 0, until we implemented an allocator)
        memory_range.offset = 0;
        // Set the size of this memory range
        memory_range.size = n_bytes;

        // Now flush the memory
        if (vkFlushMappedMemoryRanges(this->device, 1, &memory_range) != VK_SUCCESS) {
            DLOG(fatal, "Could not flush mapped memory region back to device.");
        }
    }

    // With that done, unmap the memory
    vkUnmapMemory(this->device, this->vk_memory);

    DLEAVE;
}

/* Populates the buffer through a temporary staging buffer. Note that this buffer is recommended to have the VK_USAGE_TRANSFER_DST_BIT set, but it's not necessary. */
void Buffer::set_staging(void* data, size_t data_size, CommandPool& command_pool) {
    DENTER("Vulkan::Buffer::set_staging");

    // Create the staging buffer
    Buffer staging_buffer(this->device, (VkDeviceSize) data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Populate it with the given data
    staging_buffer.set(data, data_size);

    // Use the copy call to move it to this buffer
    Buffer::copy(*this, staging_buffer, command_pool);

    // We're done (deallocation is done by RAII)
    DLEAVE;
}

/* Returns the contents of the buffer by copying it in the given void pointer. Note that the buffer has to have the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT flag set, and that it's assumed that the given array is at least size() bytes long. */
void Buffer::get(void* data) {
    DENTER("Vulkan::Buffer::get");

    // Check if the correct bit is set
    if (!(this->vk_mem_property_flags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)) {
        DLOG(fatal, "Cannot read from buffer that is inaccessible by the host.");
    }

    // Try to map the memory
    void* mapped_memory;
    if (vkMapMemory(this->device, this->vk_memory, 0, this->vk_buffer_size, 0, &mapped_memory) != VK_SUCCESS) {
        DLOG(fatal, "Could not map buffer memory to host memory.");
    }

    // Force a write in the memory if the coherent bit is not set
    // Next, flush the memory to the device if the cache is not coherent
    if (!(this->vk_mem_property_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        VkMappedMemoryRange memory_range{};
        memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        // Set the mapped memory to flush
        memory_range.memory = this->vk_memory;
        // Set the offset for this memory (always 0, until we implemented an allocator)
        memory_range.offset = 0;
        // Set the size of this memory range
        memory_range.size = this->vk_buffer_size;

        // Now flush the memory
        if (vkInvalidateMappedMemoryRanges(this->device, 1, &memory_range) != VK_SUCCESS) {
            DLOG(fatal, "Could not invalidate mapped memory region.");
        }
    }

    // Copy the memory back
    memcpy(data, mapped_memory, (size_t) this->vk_buffer_size);

    // With that done, unmap the memory again
    vkUnmapMemory(this->device, this->vk_memory);

    DLEAVE;
}
