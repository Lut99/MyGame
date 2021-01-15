/* BUFFER.hpp
 *   by Lut99
 *
 * Created:
 *   14/01/2021, 15:55:48
 * Last edited:
 *   14/01/2021, 15:55:48
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

#ifndef VULKAN_BUFFER_HPP
#define VULKAN_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "CommandPool.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The Buffer class, which wraps a VkBuffer object and handles its memory. */
    class Buffer {
    private:
        /* The VkDeviceMemory that is used to actually store the data. */
        VkDeviceMemory vk_memory;
        /* The requirements set for the memory of this buffer. */
        VkMemoryPropertyFlags vk_mem_property_flags;

        /* The VkBuffer object that uses the internal memory. */
        VkBuffer vk_buffer;
        /* The number of bytes that the memory is allocated for. */
        VkDeviceSize vk_buffer_size;
        /* The usage flags set for this buffer. */
        VkBufferUsageFlags vk_usage_flags;;

    public:
        /* The device where this buffer lives. */
        const Device& device;


        /* Constructor for the Buffer class, which takes the Device to bind the buffer to, the size of the buffer (in bytes), flags describing how the buffer will be used (for optimizations), flags that mark the requirements we have for the buffer and its memory and optionally create flags for the buffer. */
        Buffer(const Device& device, VkDeviceSize n_bytes, VkBufferUsageFlags buffer_usage, VkMemoryPropertyFlags buffer_requirements, VkBufferCreateFlags buffer_flags = 0);
        /* Copy constructor for the Buffer class, which is deleted. */
        Buffer(const Buffer& other) = delete;
        /* Move constructor for the Buffer class. */
        Buffer(Buffer&& other);
        /* Destructor for the Buffer class. */
        virtual ~Buffer();

        /* Returns the appropriate memory type based on the given requirements as stated by a VkBuffer and custom requirements. */
        static uint32_t get_memory_type(const Device& device, uint32_t type_filter, VkMemoryPropertyFlags properties);
        /* Copies one Buffer to the other. Both buffers must be on the same device, and the source buffer must have VK_BUFFER_USAGE_TRANSFER_SRC_BIT and the destination buffer VK_BUFFER_TRANSFER_DST_BIT. Additionally, the destination buffer must have at least as many bytes allocated as the source one. The final argument is the command pool where we'll create the temporary command buffer for the copy. */
        static void copy(Buffer& destination, const Buffer& source, CommandPool& command_pool);

        /* Populates the buffer directly, by mapping the appropriate device memory to host memory and then copying the data in the given array. Note that to do this, this array must have VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT set. */
        void set(void* data, size_t data_size);
        /* Populates the buffer through a temporary staging buffer. Note that this buffer is recommended to have the VK_USAGE_TRANSFER_DST_BIT set, but it's not necessary. */
        void set_staging(void* data, size_t data_size, CommandPool& command_pool);
        /* Returns the contents of the buffer by copying it in the given void pointer. Note that the buffer has to have the VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT flag set, and that it's assumed that the given array is at least size() bytes long. */
        void get(void* data);

        /* Returns the size (in bytes) of this buffer. */
        inline VkDeviceSize size() const { return this->vk_buffer_size; }

        /* Returns the usage flags set for this buffer. */
        inline VkBufferUsageFlags usage() const { return this->vk_usage_flags; }
        /* Returns the properties set for the memory of this buffer. */
        inline VkMemoryPropertyFlags properties() const { return this->vk_mem_property_flags; }

        /* Expliticly retrieves the VkDeviceMemory object that this class wraps. */
        inline VkDeviceMemory memory() const { return this->vk_memory; }
        /* Expliticly retrieves the VkBuffer object that this class wraps. */
        inline VkBuffer buffer() const { return this->vk_buffer; }
        /* Implicitly casts this class to a VkBuffer, by returning the internal VkBuffer object that this class wraps. */
        inline operator VkBuffer() const { return this->vk_buffer; }

    };

    /* Swap operator for the Buffer class. */
    void swap(Buffer& b1, Buffer& b2);
}

#endif
