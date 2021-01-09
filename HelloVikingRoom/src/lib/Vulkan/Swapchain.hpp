/* SWAPCHAIN.hpp
 *   by Lut99
 *
 * Created:
 *   08/01/2021, 13:42:20
 * Last edited:
 *   08/01/2021, 13:42:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The Swapchain class is used to wrap and organise the swapchain that
 *   we'll use for our project.
**/

#ifndef VULKAN_SWAPCHAIN_HPP
#define VULKAN_SWAPCHAIN_HPP

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Application/MainWindow.hpp"
#include "Tools/Array.hpp"

namespace HelloVikingRoom::Vulkan {
    /* Wraps and manages the Vulkan Swapchain class. */
    class Swapchain {
    private:
        /* The swapchain that this class wraps. */
        VkSwapchainKHR vk_swapchain;

        /* The Frames that reside in the swapchain. */
        Tools::Array<VkImage> vk_images;
        /* The image views that are inherently linked to this swapchain. */
        Tools::Array<VkImageView> vk_imageviews;
        /* The image format we chose for this swapchain. */
        VkFormat vk_format;
        /* The extent (i.e., resolution) of the swapchain frames. */
        VkExtent2D vk_extent;

    public:
        /* The device that this Swapchain is created on. */
        const Device& device;


        /* Constructor for the Swapchain class, which takes the main window and a device to create the swapchain from. */
        Swapchain(const MainWindow& window, const Device& device);
        /* Copy constructor for the Swapchain class, which is deleted. */
        Swapchain(const Swapchain& other) = delete;
        /* Move constructor for the Swapchain class. */
        Swapchain(Swapchain&& other);
        /* Destructor for the Swapchain class. */
        ~Swapchain();

        /* Selects the appropriate swapchain format based on the given device. */
        static VkSurfaceFormatKHR select_format(const Device& device);
        /* Selects the appropriate swapchain presentation mode based on the given device. */
        static VkPresentModeKHR select_present_mode(const Device& device);
        /* Selects the appropriate swapchain resolution based on the capabilities of the given window and the chosen device. */
        static VkExtent2D select_resolution(const MainWindow& window, const Device& device);

        /* Explicitly returns a constant reference the images inside the Swapchain. */
        inline const Tools::Array<VkImage>& images() const { return this->vk_images; }
        /* Explicitly returns a constant reference to the imageviews referencing the images of the Swapchain. */
        inline const Tools::Array<VkImageView>& imageviews() const { return this->vk_imageviews; }
        /* Explititly returns the format of the images inside the Swapchain. */
        inline VkFormat format() const { return this->vk_format; }
        /* Explititly returns the resolution (extent) of the images inside the Swapchain. */
        inline VkExtent2D extent() const { return this->vk_extent; }

        /* Explicitly retrieves the internal swapchain object. */
        inline VkSwapchainKHR swapchain() const { return this->vk_swapchain; }
        /* Implicitly casts the Swapchain to its internal VkSwapchain. */
        inline operator VkSwapchainKHR() const { return this->vk_swapchain; }

    };
}

#endif
