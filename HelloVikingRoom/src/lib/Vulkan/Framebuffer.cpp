/* FRAMEBUFFER.cpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 15:21:41
 * Last edited:
 *   13/01/2021, 15:21:41
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Framebuffer class, which wraps a VkFramebuffer object to
 *   manage it RAII-style. That object, in turn, is meant to provide access
 *   to the swapchain ImageViews, but then with pipeline-specific formats.
**/

#include "Debug/Debug.hpp"
#include "Framebuffer.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** FRAMEBUFFER CLASS *****/
/* Constructor for the Framebuffer class, which takes a Device where the framebuffer lives, a Swapchain's ImageView to bind to, the extend of the Swapchain's ImageView and a RenderPass to bind the framebuffer to. */
Framebuffer::Framebuffer(const Device& device, const VkImageView& image_view, const Swapchain& swapchain, const RenderPass& render_pass) :
    device(device),
    create_info({})
{
    DENTER("Vulkan::Framebuffer::Framebuffer");
    DLOG(info, "Creating Vulkan framebuffer...");

    // Prepare the create info struct
    this->create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    // Link a renderpass to this framebuffer
    this->create_info.renderPass = render_pass;
    // Attach the imageviews
    this->create_info.attachmentCount = 1;
    this->create_info.pAttachments = &image_view;
    // Specify the size of the framebuffer (i.e., the size of the imageviews)
    this->create_info.width = swapchain.extent().width;
    this->create_info.height = swapchain.extent().height;
    // Specify how many layers there are in this image (again, will likely always be 1)
    this->create_info.layers = 1;

    // Use the regenerate function to actually create it
    this->regenerate();

    DLEAVE;
}

/* Move constructor for the Framebuffer class. */
Framebuffer::Framebuffer(Framebuffer&& other) :
    vk_framebuffer(other.vk_framebuffer),
    device(other.device),
    create_info(other.create_info)
{
    other.vk_framebuffer = nullptr;
}

/* Destructor for the Framebuffer class. */
Framebuffer::~Framebuffer() {
    DENTER("Vulkan::Framebuffer::~Framebuffer");
    DLOG(info, "Cleaning Vulkan framebuffer...");

    if (this->vk_framebuffer != nullptr) {
        vkDestroyFramebuffer(this->device, this->vk_framebuffer, nullptr);
    }

    DLEAVE;
}



/* Regenerates the Framebuffer based on the internal create info. */
void Framebuffer::regenerate() {
    DENTER("Vulkan::Framebuffer::regenerate");

    // Simply create it using our internal struct
    if (vkCreateFramebuffer(this->device, &this->create_info, nullptr, &this->vk_framebuffer) != VK_SUCCESS) {
        DLOG(fatal, "Could not create Framebuffer.");
    }

    DRETURN;
}
