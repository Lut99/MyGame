/* SWAPCHAIN.cpp
 *   by Lut99
 *
 * Created:
 *   08/01/2021, 13:42:25
 * Last edited:
 *   08/01/2021, 13:42:25
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The Swapchain class is used to wrap and organise the swapchain that
 *   we'll use for our project.
**/

#include <GLFW/glfw3.h>

#include "Debug/Debug.hpp"
#include "Swapchain.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** SWAPCHAIN CLASS *****/
/* Constructor for the Swapchain class, which takes the main window and a device to create the swapchain from. */
Swapchain::Swapchain(const MainWindow& window, const Device& device) :
    vk_swapchain(nullptr),
    device(device)
{
    DENTER("Vulkan::Swapchain::Swapchain");
    DLOG(info, "Initializing Vulkan swapchain...");

    // Select the minimum number of frames to use, which is the minimum number allowed plus at least one, unless that exceeds the device's capabilities
    //   Do note, however, that if the device reports a maximum of '0' that means no maximum
    const VkSurfaceCapabilitiesKHR& capabilities = device.get_swapchain_info().capabalities();
    uint32_t min_frames = capabilities.minImageCount + 1;
    if (    capabilities.maxImageCount > 0 &&
            min_frames > capabilities.maxImageCount) {
        min_frames = capabilities.maxImageCount;
    }

    // Fill in the create info struct for the swapchain
    this->vk_swapchain_info = {};
    this->vk_swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    // Pass the surface used for this swapChain as well
    this->vk_swapchain_info.surface = window.surface();
    // Next, tell it how many images it should have
    this->vk_swapchain_info.minImageCount = min_frames;
    // Always set to 1 layers, since we won't be creating a 3D-glasses application
    this->vk_swapchain_info.imageArrayLayers = 1;
    // We'll use the image as a direct render target (other options might be to render it to memory first for post-processing)
    this->vk_swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Next we'll talk about the queues specified
    const DeviceQueueInfo& queue_info = device.get_queue_info();
    this->vk_queue_indices = { queue_info.graphics(), queue_info.presentation() };
    if (queue_info.graphics() == queue_info.presentation()) {
        // The graphics queue and presentation queue are one and the same, and thus we tell the swapchain to let the one queue have exclusive access to the frames
        this->vk_swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        // Tell it not to share it between any queues (optional, only here for legacy reasons; the sharing mode should let the Vulkan API not touch these values)
        this->vk_swapchain_info.queueFamilyIndexCount = 0;
        this->vk_swapchain_info.pQueueFamilyIndices = nullptr;
    } else {
        // They are different queues, and thus we'll tell the swapchain to let both of them have ownership of the frames
        this->vk_swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        // Tell the swapchain to share it between the two queues
        this->vk_swapchain_info.queueFamilyIndexCount = static_cast<uint32_t>(this->vk_queue_indices.size());
        this->vk_swapchain_info.pQueueFamilyIndices = this->vk_queue_indices.rdata();
    }

    // We can specify we want a certain transformation in the swapchain (like rotating the image), but since we don't we'll just specify the current transform
    this->vk_swapchain_info.preTransform = capabilities.currentTransform;
    // We can also tell the swapchain how to treat the opacity channel. In most cases, we'll just ignore it
    this->vk_swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // If the cipped property is set to true, then the colours of pixels that are omitted by windows may be arbitrary (i.e., best performance)
    this->vk_swapchain_info.clipped = VK_TRUE;
    // This can be used to re-optimise the swapchain, but for now we'll leave it be
    this->vk_swapchain_info.oldSwapchain = VK_NULL_HANDLE;

    // Also prepare the create info for the ImageViews
    this->vk_imageview_info = {};
    this->vk_imageview_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    // Define how to interpret the images. Since it are output frames, they're 2D
    this->vk_imageview_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    // Something really sweet is that we can re-map colour channels (for example, to make the image monochrome). For now, though, we'll just use the standard mappings
    this->vk_imageview_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    this->vk_imageview_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    this->vk_imageview_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    this->vk_imageview_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    // Next up, we'll tell the imageview what we'll do with the image; since it's output, there's nothing fancy like multiple levels or mipmaps
    this->vk_imageview_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    this->vk_imageview_info.subresourceRange.baseMipLevel = 0;
    this->vk_imageview_info.subresourceRange.levelCount = 1;
    this->vk_imageview_info.subresourceRange.baseArrayLayer = 0;
    this->vk_imageview_info.subresourceRange.layerCount = 1;

    // Use the internal resize() function to create the window
    this->resize(window);

    DLEAVE;
}

/* Move constructor for the Swapchain class. */
Swapchain::Swapchain(Swapchain&& other) :
    vk_swapchain(other.vk_swapchain),
    vk_images(other.vk_images),
    vk_imageviews(other.vk_imageviews),
    vk_format(other.vk_format),
    vk_extent(other.vk_extent),
    device(other.device),
    vk_swapchain_info(other.vk_swapchain_info),
    vk_imageview_info(other.vk_imageview_info),
    vk_queue_indices(other.vk_queue_indices)
{
    // Set the destructible vulkan stuff to nullptrs
    other.vk_swapchain = nullptr;
    other.vk_imageviews.clear();
}

/* Destructor for the Swapchain class. */
Swapchain::~Swapchain() {
    DENTER("Vulkan::Swapchain::~Swapchain");
    DLOG(info, "Cleaning Vulkan swapchain...");

    // Destroy the image views first
    for (size_t i = 0; i < this->vk_imageviews.size(); i++) {
        vkDestroyImageView(this->device, this->vk_imageviews[i], nullptr);
    }

    // Destroy the swapchain if we still have one
    if (this->vk_swapchain != nullptr) {
        vkDestroySwapchainKHR(this->device, this->vk_swapchain, nullptr);
    }

    DLEAVE;
}



/* Selects the appropriate swapchain format based on the given device. */
VkSurfaceFormatKHR Swapchain::select_format(const Device& device) {
    DENTER("Vulkan::Swapchain::select_format");

    // Loop through the formats and see if our desired one is supported
    const Array<VkSurfaceFormatKHR>& formats = device.get_swapchain_info().formats();
    for (size_t i = 0; i < formats.size(); i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            // We wanna have that one!
            DRETURN formats[i];
        }
    }

    // Otherwise, let's just use the first available one (since we'll know there's at least one)
    DRETURN formats[0];
}

/* Selects the appropriate swapchain presentation mode based on the given device. */
VkPresentModeKHR Swapchain::select_present_mode(const Device& device) {
    DENTER("Vulkan::Swapchain::select_present_mode");

    // Loop through the formats and see if ours is supported
    const Array<VkPresentModeKHR>& modes = device.get_swapchain_info().present_modes();
    for (size_t i = 0; i < modes.size(); i++) {
        // We ideally want triple buffering
        if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            // We wanna have that one!
            DRETURN modes[i];
        }
    }

    // Otherwise, we'll go for the one that's guaranteed to be available (basically normal vsync)
    DRETURN VK_PRESENT_MODE_FIFO_KHR;
}

/* Selects the appropriate swapchain resolution based on the capabilities of the given window and the chosen device. */
VkExtent2D Swapchain::select_resolution(const MainWindow& window, const Device& device) {
    DENTER("Vulkan::Swapchain::select_resolution");

    // Distinguish between the special UINT32_MAX and just returning the extent in the capabilites (=resolution)
    const VkSurfaceCapabilitiesKHR& capabilities = device.get_swapchain_info().capabalities();
    if (capabilities.currentExtent.width != UINT32_MAX) {
        DRETURN capabilities.currentExtent;
    } else {
        // Get the current size of the window
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        // Store them in a Vulkan extent
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        // Next, update that extend with whichever is larger:
        //   - The minimum resolution that the swap chain supports
        //   - The smallest of either the maximum resolution supported by the swap chain or the width currently stored in it
        // In other words, make sure the resolution is bounded in between the minimum and maximum supported resolution
        if (actualExtent.width > capabilities.maxImageExtent.width) { actualExtent.width = capabilities.maxImageExtent.width; }
        if (actualExtent.width < capabilities.minImageExtent.width) { actualExtent.width = capabilities.minImageExtent.width; }
        if (actualExtent.height > capabilities.maxImageExtent.height) { actualExtent.height = capabilities.maxImageExtent.height; }
        if (actualExtent.height < capabilities.minImageExtent.height) { actualExtent.height = capabilities.minImageExtent.height; }

        // We're done here too
        DRETURN actualExtent;
    }
}



/* Regenerates the swapchain based on the new size of the given window. */
void Swapchain::resize(const MainWindow& window) {
    DENTER("Vulkan::Swapchain::resize");

    // Destroy the old ImageViews if there are any
    for (size_t i = 0; i < this->vk_imageviews.size(); i++) {
        vkDestroyImageView(this->device, this->vk_imageviews[i], nullptr);
    }
    // Destroy the old swapchain if there is one
    if (this->vk_swapchain != nullptr) {
        vkDestroySwapchainKHR(this->device, this->vk_swapchain, nullptr);
    }

    // Start by selecting the correct format, presentation mode & resolution based on our preferences (encoded in the functions) and the availability on the device
    VkSurfaceFormatKHR format = Swapchain::select_format(device);
    VkExtent2D extent = Swapchain::select_resolution(window, this->device);
    VkPresentModeKHR present_mode = Swapchain::select_present_mode(device);

    // Store some of that data locally
    this->vk_format = format.format;
    this->vk_extent = extent;
    DLOG(auxillary, "Selected swapchain size: " + std::to_string(this->vk_extent.width) + "x" + std::to_string(this->vk_extent.height));

    // Update it in the create info
    this->vk_swapchain_info.imageFormat = this->vk_format;
    this->vk_swapchain_info.imageColorSpace = format.colorSpace;
    this->vk_swapchain_info.imageExtent = this->vk_extent;
    this->vk_swapchain_info.presentMode = present_mode;

    // Use that to create a new one
    if (vkCreateSwapchainKHR(this->device, &this->vk_swapchain_info, nullptr, &this->vk_swapchain) != VK_SUCCESS) {
        DLOG(fatal, "Could not create swapchain.");
    }

    // Update the handles from the freshly created swapchain. Note that its size may have changed.
    uint32_t n_frames;
    if (vkGetSwapchainImagesKHR(this->device, this->vk_swapchain, &n_frames, nullptr) != VK_SUCCESS) {
        DLOG(fatal, "Could not get swapchain image count.");
    }
    this->vk_images.reserve(n_frames);
    if (vkGetSwapchainImagesKHR(this->device, this->vk_swapchain, &n_frames, this->vk_images.wdata(n_frames)) != VK_SUCCESS) {
        DLOG(fatal, "Could not get images from swapchain.");
    }



    // Also update the format in the ImageView's create info
    this->vk_imageview_info.format = this->vk_format;
    
    // Re-create all the imageviews
    this->vk_imageviews.resize(n_frames);
    for (uint32_t i = 0; i < n_frames; i++) {
        // Update he image we're talking about
        this->vk_imageview_info.image = this->vk_images[i];

        // Now, create the image view for this image
        if (vkCreateImageView(this->device, &this->vk_imageview_info, nullptr, &(this->vk_imageviews[i])) != VK_SUCCESS) {
            DLOG(fatal, "Failed to create ImageView for image " + std::to_string(i) + " in the swapchain.");
        }
    }

    DRETURN;
}
