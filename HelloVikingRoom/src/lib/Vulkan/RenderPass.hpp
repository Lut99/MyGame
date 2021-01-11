/* RENDER PASS.hpp
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
 *   The Renderpass class is the baseclass used to define other, more specific
 *   render passes.
**/

#ifndef VULKAN_RENDERPASS_HPP
#define VULKAN_RENDERPASS_HPP

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Tools/Array.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The RenderPass class, which is used to wrap around a normal RenderPass for easy resource management. */
    class RenderPass {
    protected:
        /* The internal VkRenderPass object that this class wraps. */
        VkRenderPass vk_render_pass;

        /* Constructor for the RenderPass class, which only takes the device this render pass is bound to. */
        RenderPass(const Device& device);

    public:
        /* Constant reference to the device where this render pass is defined. */
        const Device& device;
        /* The list of VkAttachmentDescription structs used during the render creation (used for easy re-creation of the RenderPass). */
        Tools::Array<VkAttachmentDescription> vk_attachments;
        /* The list of VkAttachmentReference structs used during the render creation (used for easy re-creation of the RenderPass). */
        Tools::Array<VkAttachmentReference> vk_attachments_refs;
        /* The list of VkSubpassDescription structs used during the render creation (used for easy re-creation of the RenderPass). */
        Tools::Array<VkSubpassDescription> vk_subpasses;
        /* The list of VkSubpassDependency structs used during the render creation (used for easy re-creation of the RenderPass). */
        Tools::Array<VkSubpassDependency> vk_subpasses_dependencies;


        /* Copy constructor for the RenderPass class, which is deleted as it makes no sense to copy a handle. */
        RenderPass(const RenderPass& other) = delete;
        /* Move constructor for the RenderPass class. */
        RenderPass(RenderPass&& other);
        /* Destructor for the RenderPass class. */
        virtual ~RenderPass();

        /* Virtual function that re-generates the RenderPass from the internal structs. Assumes the device is not currently using this RenderPass. */
        virtual void regenerate() = 0;

        /* Explicitly retrieves a constant refrence to the internal VkRenderPass object. */
        inline VkRenderPass render_pass() const { return this->vk_render_pass; }
        /* Implicitly converts this RenderPass object to a VkRenderPass object, by returning the internal copy we wrap. */
        inline operator VkRenderPass() const { return this->vk_render_pass; }

    };
}

#endif
