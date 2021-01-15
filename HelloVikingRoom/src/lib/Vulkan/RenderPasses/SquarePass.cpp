/* SQUARE PASS.cpp
 *   by Lut99
 *
 * Created:
 *   11/01/2021, 17:37:09
 * Last edited:
 *   11/01/2021, 17:37:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The SquarePass class is the RenderPass specifically for drawing a
 *   square on the screen as seen in HelloTriangle. Derives from the
 *   RenderPass class.
**/

#include "Debug/Debug.hpp"
#include "SquarePass.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan::RenderPasses;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** SQUAERPASS CLASS *****/
/* Constructor for the SquarePass class, which takes the device this render pass is bound to and the swapchain to take the image format from. */
SquarePass::SquarePass(const Device& device, const Swapchain& swapchain) :
    RenderPass(device)
{
    DENTER("Vulkan::RenderPasses::SquarePass::SquarePass");
    DLOG(info, "Initializing Vulkan SquarePass render pass...");

    // Start by defining the VkAttachementDescription. We only have one framebuffer with one image, so we also only need one attachment
    this->vk_attachments.push_back({});
    // We don't do multisampling (yet)
    this->vk_attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    // We define what we want to do with the frame when we load it; clear it with a constant (since we'll be adding alphas to it)
    this->vk_attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    // When writing the image again, we want to store it to be read later (by the monitor, for example)
    this->vk_attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    // We don't do anything with the stencil buffer, so we couldn't care less
    this->vk_attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    this->vk_attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    // For textures, we have to define the layout of the image at the start and at the end. We don't care what it is when we begin...
    this->vk_attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // ...but when we're done, it must have a presenteable format
    this->vk_attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // We have to define a reference to the attachment above. We use only one subpass, i.e., one reference to the first index
    this->vk_attachments_refs.push_back({});
    // We reference the first and only attachment (created above)
    this->vk_attachments_refs[0].attachment = 0;
    // We also need to define the layout that the framebuffer has for this subpass. We'll tell it to optimize for colours
    this->vk_attachments_refs[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Now we combine the reference(s) in a subpass
    this->vk_subpasses.push_back({});
    // Tell Vulkan to treat it as a graphics pass. In the future (which may be now, who knows) it might support compute passes
    this->vk_subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    // Add the color reference as an array
    this->vk_subpasses[0].colorAttachmentCount = static_cast<uint32_t>(this->vk_attachments_refs.size());
    this->vk_subpasses[0].pColorAttachments = this->vk_attachments_refs.rdata();
    // Note that others may be added, which can be used to read from in shaders, multisampling, depth / stencils and passing data to next subpasses

    // Next, create a dependency for this subpass, namely that we have to acquired the image before we can write to it
    this->vk_subpasses_dependencies.push_back({});
    // The VK_SUBPASS_EXTERNAL value indicates that we're waiting for the implicit subpass stage before this one
    this->vk_subpasses_dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    // Let it know that we'll start with the first subpass once the dependency is matched
    this->vk_subpasses_dependencies[0].dstSubpass = 0;
    // Next, we specify that we want to wait for the image-ready event but not for any accessses
    this->vk_subpasses_dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    this->vk_subpasses_dependencies[0].srcAccessMask = 0;
    // Then, we define that we only want to begin when the image is ready to write to
    this->vk_subpasses_dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    this->vk_subpasses_dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    // Use our internal regeneration function to do the final steps
    this->resize(swapchain);

    // Done!
    DLEAVE;
}

/* Move constructor for the SquarePass class. */
SquarePass::SquarePass(SquarePass&& other) :
    RenderPass((RenderPass&&) other)
{}

/* Destructor for the SquarePass class. */
SquarePass::~SquarePass() {
    DENTER("Vulkan::RenderPasses::SquarePass::~SquarePass");
    DLOG(info, "Cleaning Vulkan SquarePass render pass...");
    DLEAVE;
}



/* Virtual function that re-generates the RenderPass from the internal structs. Assumes the device is not currently using this RenderPass. */
void SquarePass::resize(const Swapchain& swapchain) {
    DENTER("Vulkan::RenderPasses::SquarePass::resize");

    // Destroy the old one if there is one
    if (this->vk_render_pass != nullptr) {
        vkDestroyRenderPass(this->device, this->vk_render_pass, nullptr);
    }

    // Update the format of the window in the render pass
    this->vk_attachments[0].format = swapchain.format();

    // Create the create info for this pass
    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    // Tell it we have one attachment
    render_pass_info.attachmentCount = static_cast<uint32_t>(this->vk_attachments.size());
    render_pass_info.pAttachments = this->vk_attachments.rdata();
    // Tell it we have one subpass
    render_pass_info.subpassCount = static_cast<uint32_t>(this->vk_subpasses.size());
    render_pass_info.pSubpasses = this->vk_subpasses.rdata();
    // Add the dependency to the subphase
    render_pass_info.dependencyCount = static_cast<uint32_t>(this->vk_subpasses_dependencies.size());;
    render_pass_info.pDependencies = this->vk_subpasses_dependencies.rdata();

    // Use that to generate a new one
    if (vkCreateRenderPass(this->device, &render_pass_info, nullptr, &this->vk_render_pass) != VK_SUCCESS) {
        DLOG(fatal, "Could not create render pass.");
    }

    DRETURN;
}
