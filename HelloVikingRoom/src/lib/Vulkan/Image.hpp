/* IMAGE.hpp
 *   by Lut99
 *
 * Created:
 *   16/01/2021, 15:26:46
 * Last edited:
 *   16/01/2021, 15:26:46
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Image class, which is responsible for loading and
 *   managing textures in our program via VkImage classes.
**/

#ifndef VULKAN_IMAGE_HPP
#define VULKAN_IMAGE_HPP

#include <vulkan/vulkan.h>
#include <string>

#include "Vulkan/Device.hpp"
#include "Vulkan/CommandPool.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The Image class, which loads and manages texture files using the stb image library. */
    class Image {
    private:
        /* The VkImage class that this class wraps. */
        VkImage vk_image;
        /* The memory allocated on the device for this image. */
        VkDeviceMemory vk_memory;

        /* The VkImageView object used to, well, view this image. */
        VkImageView vk_image_view;

        /* The current size of the image. */
        VkExtent2D vk_extent;
        /* The current format of the image. */
        VkFormat vk_format;
        /* The current layout of the image. */
        VkImageLayout vk_layout;
    
    public:
        /* Constant reference to the device where the image lives. */
        const Device& device;

        /* Constructor for the Image class, which takes the device where to put the image, the command pool used to perform operations, the path to the texture file to load. Optionally takes extra usage flags and extra memory requirements for the image. */
        Image(const Device& device, CommandPool& command_pool, const std::string& texture_path, VkImageUsageFlags usage_flags = 0, VkMemoryPropertyFlags property_flags = 0);
        /* Copy constructor for the Image class, which is deleted. */
        Image(const Image& other) = delete;
        /* Move constructor for the Image class. */
        Image(Image&& other);
        /* Destructor for the Image class. */
        ~Image();

        /* Populates the image with the contents of the given Buffer. */
        static void copy(Image& destination, const Buffer& source, CommandPool& command_pool);

        /* Transitions the image from its current layout to a new one. Adds in a barrier to make sure the pipeline only continues when the image has the right layout. */
        void transition_layout(const VkImageLayout& new_layout, CommandPool& command_pool);

        /* Returns the size of the image as a VkExtent2D object. */
        inline const VkExtent2D extent() const { return this->vk_extent; }
        /* Returns the current format of the image. */
        inline VkFormat format() const { return this->vk_format; }
        /* Returns the current layout of the image. */
        inline VkImageLayout layout() const { return this->vk_layout; }

        /* Expliticly returns the internal VkImage object. */
        inline const VkImage& image() const { return this->vk_image; }
        /* Explicitly returns the internal VkDeviceMemory object. */
        inline const VkDeviceMemory& memory() const { return this->vk_memory; }
        /* Explicitly returns the internal VkImageView object. */
        inline const VkImageView& image_view() const { return this->vk_image_view; }
        /* Implicitly casts this class to a VkImage by returning the internal object. */
        inline operator VkImage() const { return this->vk_image; }
        /* Implicitly casts this class to a VkDeviceMemory by returning the internal object. */
        inline operator VkDeviceMemory() const { return this->vk_memory; }
        /* Implicitly casts this class to a VkImageView by returning the internal object. */
        inline operator VkImageView() const { return this->vk_image_view; }
        
    };
}

#endif
