/* IMAGE.cpp
 *   by Lut99
 *
 * Created:
 *   16/01/2021, 15:26:49
 * Last edited:
 *   16/01/2021, 15:26:49
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Image class, which is responsible for loading and
 *   managing textures in our program via VkImage classes.
**/

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "Vulkan/Buffer.hpp"
#include "Debug/Debug.hpp"
#include "Image.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** IMAGE CLASS *****/
/* Constructor for the Image class, which takes the device where to put the image, the command pool used to perform operations, the path to the texture file to load. Optionally takes extra usage flags and extra memory requirements for the image. */
Image::Image(const Device& device, CommandPool& command_pool, const std::string& texture_path, VkImageUsageFlags usage_flags, VkMemoryPropertyFlags property_flags) :
    vk_extent({}),
    vk_format(VK_FORMAT_R8G8B8A8_SRGB),
    vk_layout(VK_IMAGE_LAYOUT_UNDEFINED),
    device(device)
{
    DENTER("Vulkan::Image::Image");
    DLOG(info, "Creating Vulkan image...");

    /***** STEP 1: LOAD TEXTURE TO BUFFER *****/
    // Start, however, by loading the raw image from file
    DLOG(auxillary, "Loading image from file '" + texture_path + "'...");
    int texture_width, texture_height, texture_channels;
    stbi_uc* texels = stbi_load(texture_path.c_str(), &texture_width, &texture_height, &texture_channels, STBI_rgb_alpha);

    // Copy the width & height to our extent
    this->vk_extent.width = static_cast<uint32_t>(texture_width);
    this->vk_extent.height = static_cast<uint32_t>(texture_height);

    // Next, use that to populate a staging buffer to load our image
    Buffer buffer(device, (VkDeviceSize) texture_width * texture_height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    buffer.set((void*) texels, (size_t) texture_width * texture_height * 4);

    // We can free the host-side memory at this point
    stbi_image_free(texels);



    /***** STEP 2: CREATE THE IMAGE *****/
    // Let's use the standard create info way to define our image
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    // Set the type of our image (just a 2D texture)
    image_info.imageType = VK_IMAGE_TYPE_2D;
    // Set the extent of our image (1 layer, since we're not doing 3D-glasses 3D)
    image_info.extent.width = this->vk_extent.width;
    image_info.extent.height = this->vk_extent.height;
    image_info.extent.depth = 1;
    // Set the number of minmaps (1 for now)
    image_info.mipLevels = 1;
    // Also, we're not using it as an array of images
    image_info.arrayLayers = 1;
    // Set the format of our image (8-bit RGBA)
    image_info.format = this->vk_format;
    // Set the tiling for our image, which we leave for Vulkan to decide the best one
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    // What to do with our initial access to the image
    image_info.initialLayout = this->vk_layout;
    // Just as with buffer, set our usage for this image
    image_info.usage = usage_flags | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // The sharing mode is private to one queue only
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // We set the multisampling to 1, which isn't relevant for textures anyway
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    // Finally, we set optional flags which relate to sparse images, but that's not relevant now
    image_info.flags = 0;

    // Use that create info to define the image
    if (vkCreateImage(this->device, &image_info, nullptr, &this->vk_image) != VK_SUCCESS) {
        DLOG(fatal, "Could not create image object.");
    }

    // Next, allocate the required memory for the image
    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(this->device, this->vk_image, &memory_requirements);

    // Use the image's memory requirements plus our own to define the memory
    VkMemoryAllocateInfo allocate_info{};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    // Set the size of the allocation
    allocate_info.allocationSize = memory_requirements.size;
    // Set the index of the type. Use the buffer's static function for this!
    allocate_info.memoryTypeIndex = Buffer::get_memory_type(this->device, memory_requirements.memoryTypeBits, property_flags);
    if (vkAllocateMemory(this->device, &allocate_info, nullptr, &this->vk_memory) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate memory for image on device.");
    }
    // Bind the newly allocated memory to our image. Note that the offset for now is zero, but this should change once/if we implement our custom allocator
    if (vkBindImageMemory(this->device, this->vk_image, this->vk_memory, 0) != VK_SUCCESS) {
        DLOG(fatal, "Could not bind memory to image.");
    }



    /***** STEP 3: COPY THE BUFFER TO THE IMAGE *****/
    // First, make sure the image is in the correct layout for copying pixels to
    this->transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, command_pool);

    // Next, use our static copyer to do the rest
    Image::copy(*this, buffer, command_pool);

    // With that out of the way, transition the image to optimized shader access
    this->transition_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, command_pool);



    // Done
    DLEAVE;
}

/* Move constructor for the Image class. */
Image::Image(Image&& other) :
    vk_image(other.vk_image),
    vk_memory(other.vk_memory),
    device(other.device)
{
    other.vk_image = nullptr;
    other.vk_memory = nullptr;
}

/* Destructor for the Image class. */
Image::~Image() {
    DENTER("Vulkan::Image::~Image");
    DLOG(info, "Cleaning Vulkan image...");

    if (this->vk_image != nullptr) {
        vkDestroyImage(this->device, this->vk_image, nullptr);
    }
    if (this->vk_memory != nullptr) {
        vkFreeMemory(this->device, this->vk_memory, nullptr);
    }

    DLEAVE;
}



/* Populates the image with the contents of the given Buffer. */
void Image::copy(Image& destination, const Buffer& source, CommandPool& command_pool) {
    DENTER("Vulkan::Image::copy");

    // Get a command buffer to run this operation on
    CommandBuffer command_buffer = command_pool.get_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // Start by defining the copy by a struct
    VkBufferImageCopy copy_info{};
    // Specify the offset of our buffer
    copy_info.bufferOffset = source.offset();
    // Define if there's any padding between the pixels in our buffer. Not in our case, though
    copy_info.bufferRowLength = 0;
    copy_info.bufferImageHeight = 0;
    // Define what to copy from the image (especially which layers etc)
    copy_info.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy_info.imageSubresource.mipLevel = 0;
    copy_info.imageSubresource.baseArrayLayer = 0;
    copy_info.imageSubresource.layerCount = 1;
    // Specify where in the image we'll place our result (just the whole image)
    copy_info.imageOffset = {0, 0, 0};
    copy_info.imageExtent.width = destination.vk_extent.width;
    copy_info.imageExtent.height = destination.vk_extent.height;
    copy_info.imageExtent.depth = 1;

    // Record it on the buffer
    vkCmdCopyBufferToImage(
        command_buffer,
        source,
        destination,
        destination.vk_layout,
        1, &copy_info
    );

    // Stop recording, and once the queue is done with everything including this operation, return
    command_buffer.end(destination.device.graphics_queue());
    DRETURN;
}



/* Transitions the image from its current layout to a new one. Adds in a barrier to make sure the pipeline only continues when the image has the right layout. */
void Image::transition_layout(const VkImageLayout& new_layout, CommandPool& command_pool) {
    DENTER("Vulkan::image::transition_layout");

    // Get a command buffer to do all this in
    CommandBuffer command_buffer = command_pool.get_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
    command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    // Begin describing the barrier that handles the transition
    VkImageMemoryBarrier image_barrier{};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    // Set from which layout we come and to which we go
    image_barrier.oldLayout = this->vk_layout;
    image_barrier.newLayout = new_layout;
    // Next, possible transfer ownership to a new queue family. Here, though, we won't
    image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    // Set the image that will be transitioned and which part(s) of the image. For use, that's just everything
    image_barrier.image = this->vk_image;
    image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = 1;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;

    // Depending on the exact transition, define the correct access masks and pipeline stages
    VkPipelineStageFlags source_stage, destination_stage;
    if (this->vk_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        // Set the barrier to not require anything, but to wait until the image is writeable
        image_barrier.srcAccessMask = 0;
        image_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        // Set the source stage as the start of the pipe, and the destination stage before we'll transfer stuff to the image
        source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (this->vk_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        // Set the barrier to require the image to be writeable, and be done before being readable by a shader
        image_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        image_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        // Set the source stage as after the transfer to the image has been completed, and the destination stage to before the fragment shader can read from it
        source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        DLOG(fatal, "Unknown image layout transformation.");
    }

    // Submit the pipeline barrier with the relevant function
    vkCmdPipelineBarrier(
        command_buffer,
        // Specify the pipeline stage after which the barrier will be places (i.e., the operations needs this to be here)
        source_stage,
        // Specify the pipeline stage before which the barrier will be placed (i.e., that needs the result)
        destination_stage,
        // Can either be nothing or VK_DEPENDENCY_BY_REGION_BIT. The latter tells Vulkan that the image can already be partly used if parts of it are ready
        0,
        // Specify the memory barriers that will be passed (none)
        0, nullptr,
        // Specify the buffer barriers that will be passed (none)
        0, nullptr,
        // Specify the image barrier that will be passed (yes, that's ours)
        1, &image_barrier
    );

    // Submit the command buffer with this operation, waiting for it to finish
    command_buffer.end(this->device.graphics_queue());

    // When that's done, update the current layout in the image
    this->vk_layout = new_layout;

    DRETURN;
}
