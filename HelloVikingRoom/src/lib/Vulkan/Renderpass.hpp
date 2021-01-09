/* RENDERPASS.hpp
 *   by Lut99
 *
 * Created:
 *   09/01/2021, 13:59:38
 * Last edited:
 *   09/01/2021, 13:59:38
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The Renderpass class is used to define render passes. Unlike other
 *   Vulkan classes, such as the Instance or Device, a lot less is
 *   hardcoded, but instead the class provides more of a C++,
 *   RAII-compliant interface to the Vulkan class than that its a really
 *   separate entitity.
**/

#ifndef VULKAN_RENDERPASS_HPP
#define VULKAN_RENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "Tools/Array.hpp"

namespace HelloVikingRoom::Vulkan {
    /* Auxillary class that is used to define the VkColorAttachment class, except it's slightly more convenient. Conceptually, this class defines how a renderpass writes to and read from buffers. */
    class AttachmentDescription {
    private:
        /* The VkAttachmentDescription that this class wraps. */
        VkAttachmentDescription vk_attachment;

    public:
        /* Constructor for the AttachmentDescription class, which takes most of the required parameters in one go. */
        AttachmentDescription();

        /* Explicitly returns a (muteable) reference to the internal VkAttachmentDescription class that this class wraps. */
        inline VkAttachmentDescription& raw() { return this->vk_attachment; }
        /* Explicitly returns a constant reference to the internal VkAttachmentDescription class that this class wraps. */
        inline const VkAttachmentDescription& raw() const { return this->vk_attachment; }

        /* Implicitly casts this class to a (muteable) VkAttachmentDescription class by returning the internal class. */
        inline operator VkAttachmentDescription() { return this->vk_attachment; }
        /* Implicitly casts this class to a constant VkAttachmentDescription class by returning the internal class. */
        inline operator VkAttachmentDescription() const { return this->vk_attachment; }

    };
}

#endif
