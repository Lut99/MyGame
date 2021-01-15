/* SQUARE PASS.hpp
 *   by Lut99
 *
 * Created:
 *   11/01/2021, 17:32:58
 * Last edited:
 *   11/01/2021, 17:32:58
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The SquarePass class is the RenderPass specifically for drawing a
 *   square on the screen as seen in HelloTriangle. Derives from the
 *   RenderPass class.
**/

#ifndef VULKAN_RENDERPASS_SQUARE_PASS_HPP
#define VULKAN_RENDERPASS_SQUARE_PASS_HPP

#include "Vulkan/RenderPass.hpp"

namespace HelloVikingRoom::Vulkan::RenderPasses {
    /* The SquarePass class is used to build the renderpass used for drawing the HelloTriangle-square on the screen. */
    class SquarePass: public RenderPass {
    public:
        /* Constructor for the SquarePass class, which takes the device this render pass is bound to and the swapchain to take the image format from. */
        SquarePass(const Device& device, const Swapchain& swapchain);
        /* Copy constructor for the SquarePass class, which is deleted (just like its parent). */
        SquarePass(const SquarePass& other) = delete;
        /* Move constructor for the SquarePass class. */
        SquarePass(SquarePass&& other);
        /* Destructor for the SquarePass class. */
        virtual ~SquarePass();

        /* Virtual function that re-generates the RenderPass from the internal structs. Assumes the device is not currently using this RenderPass. */
        virtual void resize(const Swapchain& swapchain);

    };
}

#endif
