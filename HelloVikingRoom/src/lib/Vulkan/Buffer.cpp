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
void Buffer::copy(Buffer& destination, const Buffer& source, const CommandPool& command_pool) {
    DENTER("Vulkan::Buffer::copy");

    // First, check if the device, bits and size matches
    if (&destination.device != &source.device) {
        DLOG(fatal, "Destination and source buffers do not live on the same device.");
    }
    if (destination.size() < source.size()) {
        DLOG(fatal, "Destination buffer is not large enough to receive the source buffer.");
    }
    if (!(destination.usage() & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
        DLOG(fatal, "Destination buffer does not have the VK_BUFFER_USAGE_TRANSFER_DST_BIT set.");
    }
    if (!(source.usage() & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
        DLOG(fatal, "Destination buffer does not have the VK_BUFFER_USAGE_TRANSFER_DST_BIT set.");
    }

    // Prepare a command buffer that we'll use once to preform the copy
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    // Tell it that this is a primary command buffer, i.e., one that can be invoked by us instead of another command buffer
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    // The command pool where the buffer will be created
    alloc_info.commandPool = command_pool;
    // Tell it how many buffers to allocate
    alloc_info.commandBufferCount = 1;

    // Actually perform the allocation
    VkCommandBuffer command_buffer;
    if (vkAllocateCommandBuffers(destination.device, &alloc_info, &command_buffer) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate temporary copy command buffer.");
    }

    // Start recording this command buffer right away
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // Tell it we'll only be needin' the buffer once
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    // Start the record
    if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS) {
        DLOG(fatal, "Could not begin recording in temporary command buffer.");
    }

    // Schedule the copy
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = source.size();
    vkCmdCopyBuffer(command_buffer, source, destination, 1, &copy_region);

    // Since that's all we wanna do, we're done here
    vkEndCommandBuffer(command_buffer);

    // Next, we'll submit the command buffer to the graphics queue so it can copy
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    // Submit the command buffer, and then wait until the copying is done
    if (vkQueueSubmit(destination.device.graphics_queue(), 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
        DLOG(fatal, "Could not submit temporary command buffer to graphics queue.");
    }
    if (vkQueueWaitIdle(destination.device.graphics_queue()) != VK_SUCCESS) {
        DLOG(fatal, "Something went wrong while waiting for graphics queue to finish copying.");
    }

    // Now we're done, free the command buffer since we won't need it anymore
    vkFreeCommandBuffers(destination.device, command_pool, 1, &command_buffer);

    DLEAVE;
}