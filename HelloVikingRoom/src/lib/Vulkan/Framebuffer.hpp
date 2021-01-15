/* FRAMEBUFFER.hpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 15:21:28
 * Last edited:
 *   13/01/2021, 15:21:28
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Framebuffer class, which wraps a VkFramebuffer object to
 *   manage it RAII-style. That object, in turn, is meant to provide access
 *   to the swapchain ImageViews, but then with pipeline-specific formats.
**/

#ifndef VULKAN_FRAMEBUFFER_HPP
#define VULKAN_FRAMEBUFFER_HPP

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Swapchain.hpp"
#include "RenderPass.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The Framebuffer class is used to interact with a Swapchain's ImageViews, but then in a pipeline-specific way. */
    class Framebuffer {
    private:
        /* The VkFramebuffer object wrapped by this class. */
        VkFramebuffer vk_framebuffer;
    
    public:
        /* Constant reference to the device where the framebuffer lives. */
        const Device& device;
        /* The create info struct for the Framebuffer, which can be used to re-create it quickly. */
        VkFramebufferCreateInfo create_info;

        /* Constructor for the Framebuffer class, which takes a Device where the framebuffer lives, a Swapchain's ImageView to bind to, the extend of the Swapchain's ImageView and a RenderPass to bind the framebuffer to. */
        Framebuffer(const Device& device, const VkImageView& image_view, const Swapchain& swapchain, const RenderPass& render_pass);
        /* Copy constructor for the Framebuffer class, which is deleted. */
        Framebuffer(const Framebuffer& other) = delete;
        /* Move constructor for the Framebuffer class. */
        Framebuffer(Framebuffer&& other);
        /* Destructor for the Framebuffer class. */
        ~Framebuffer();

        /* Regenerates the Framebuffer based on the internal create info. */
        void resize(const VkImageView& image_view, const Swapchain& swapchain, const RenderPass& render_pass);

        /* Expliticly returns the internal VkFramebuffer object. */
        inline VkFramebuffer framebuffer() const { return this->vk_framebuffer; }
        /* Implicitly converts this class to a VkFramebuffer class by returning the internal one. */
        inline operator VkFramebuffer() const { return this->vk_framebuffer; }
    
    };
}

#endif
