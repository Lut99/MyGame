/* SQUARE PIPELINE.hpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 14:11:51
 * Last edited:
 *   13/01/2021, 14:11:51
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SquarePipeline class, which is a specialized pipeline for
 *   rendering the simple square from HelloTriangle. Derives from the
 *   GraphicsPipeline class.
**/

#ifndef VULKAN_GRAPHICSPIPELINE_SQUARE_PIPELINE_HPP
#define VULKAN_GRAPHICSPIPELINE_SQUARE_PIPELINE_HPP

#include "Vulkan/Swapchain.hpp"
#include "Vulkan/GraphicsPipeline.hpp"

namespace HelloVikingRoom::Vulkan::GraphicsPipelines {
    /* The SquarePipeline class, which defines the pipeline that is used to render the square from HelloTriangle. */
    class SquarePipeline: public GraphicsPipeline {
    public:
        /* Constructor for the SquarePipeline class, which takes the device to create the pipeline on, a swapchain to deduce the image format from and a render pass to render in the pipeline. */
        SquarePipeline(const Device& device, const Swapchain& swapchain, const RenderPass& render_pass);
        /* Copy constructor for the SquarePipeline class, which is deleted. */
        SquarePipeline(const SquarePipeline& other) = delete;
        /* Move constructor for the SquarePipeline class. */
        SquarePipeline(SquarePipeline&& other);
        /* Destructor for the SquarePipeline class. */
        virtual ~SquarePipeline();

        /* Virtual function to re-create the pipeline, based on the internally stored structs. Takes a render pass to render in this pipeline */
        virtual void regenerate(const RenderPass& render_pass);

    };
}

#endif
