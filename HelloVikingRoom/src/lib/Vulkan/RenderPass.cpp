/* RENDER PASS.cpp
 *   by Lut99
 *
 * Created:
 *   09/01/2021, 13:59:28
 * Last edited:
 *   09/01/2021, 13:59:28
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The Renderpass class is the baseclass used to define other, more specific
 *   render passes.
**/

#include "Debug/Debug.hpp"

#include "RenderPass.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** RENDERPASS CLASS *****/
/* Constructor for the RenderPass class, which takes the device to render on, a list to attachments, a list of subpasses and a list of dependencies to define this render pass. */
RenderPass::RenderPass(const Device& device) :
    vk_render_pass(nullptr),
    device(device),
    vk_attachments({}),
    vk_attachments_refs({}),
    vk_subpasses({}),
    vk_subpasses_dependencies({})
{}

/* Move constructor for the RenderPass class. */
RenderPass::RenderPass(RenderPass&& other) :
    vk_render_pass(other.vk_render_pass),
    device(other.device),
    vk_attachments(other.vk_attachments),
    vk_attachments_refs(other.vk_attachments_refs),
    vk_subpasses(other.vk_subpasses),
    vk_subpasses_dependencies(other.vk_subpasses_dependencies)
{
    other.vk_render_pass = nullptr;
}

/* Destructor for the RenderPass class. */
RenderPass::~RenderPass() {
    DENTER("Vulkan::RenderPass::~RenderPass");

    // Only destroy if still present
    if (this->vk_render_pass != nullptr) {
        vkDestroyRenderPass(this->device, this->vk_render_pass, nullptr);
    }

    DLEAVE;
}
