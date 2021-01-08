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
using namespace HelloVikingRoom;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** SWAPCHAIN CLASS *****/
/* Constructor for the Swapchain class, which takes the main window and a device to create the swapchain from. */
Vulkan::Swapchain::Swapchain(const MainWindow& window, const Device& device) :
    device(device)
{
    DENTER("Vulkan::Swapchain::Swapchain");
    DLOG(info, "Initializing Vulkan swapchain...");



    DLEAVE;
}

/* Move constructor for the Swapchain class. */
Vulkan::Swapchain::Swapchain(Swapchain&& other) :
    vk_swapchain(other.vk_swapchain),
    vk_images(other.vk_images),
    vk_format(other.vk_format),
    vk_extent(other.vk_extent),
    device(other.device)
{
    // Set the destructible vulkan stuff to nullptrs
    other.vk_swapchain = nullptr;
    other.vk_images.clear();
}

/* Destructor for the Swapchain class. */
Vulkan::Swapchain::~Swapchain() {
    DENTER("Vulkan::Swapchain::~Swapchain");
    DLOG(info, "Cleaning Vulkan swapchain...");

    // Destroy all images in the list
    for (size_t i = 0; i < this->vk_images.size(); i++) {
        vkDestroyImage(this->device, this->vk_images[i], nullptr);
    }

    // Destroy the swapchain if we still have one
    if (this->vk_swapchain != nullptr) {
        vkDestroySwapchainKHR(this->device, this->vk_swapchain, nullptr);
    }

    DLEAVE;
}



/* Selects the appropriate swapchain format based on the given device. */
VkSurfaceFormatKHR Vulkan::Swapchain::select_format(const Device& device) {
    DENTER("Swapchain::select_format");

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
VkPresentModeKHR Vulkan::Swapchain::select_present_mode(const Device& device) {
    DENTER("Swapchain::select_present_mode");

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
VkExtent2D Vulkan::Swapchain::select_resolution(const MainWindow& window, const Device& device) {
    DENTER("Swapchain::select_resolution");

    // Distinguish between the special UINT32_MAX and just returning the extent in the capabilites (=resolution)
    const VkSurfaceCapabilitiesKHR& capabilities = device.get_swapchain_info().capabalities();
    if (capabilities.currentExtent.width != UINT32_MAX) {
        DRETURN capabilities.currentExtent;
    }
    else {
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
