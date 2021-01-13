/* GRAPHICS PIPELINE.hpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 13:32:23
 * Last edited:
 *   13/01/2021, 13:32:23
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the GraphicsPipeline class, which forms the basis for all
 *   graphics pipelines. The individual pipelines are derived as their own
 *   classes.
**/

#ifndef VULKAN_GRAPHICS_PIPELINE_HPP
#define VULKAN_GRAPHICS_PIPELINE_HPP

#include <vulkan/vulkan.h>

#include "Tools/Array.hpp"
#include "Device.hpp"
#include "RenderPass.hpp"
#include "ShaderModule.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The GraphicsPipeline class, which forms the baseclass for all graphics pipelines used. */
    class GraphicsPipeline {
    protected:
        /* The internal VkPipeline object that this class wraps. */
        VkPipeline vk_pipeline;
        /* The internal VkPipelineLayout object that this class also happens to wrap. */
        VkPipelineLayout vk_pipeline_layout;
    
    public:
        /* Constant reference to the device to which the graphics pipeline is bound. */
        const Device& device;

        /* Array of shader modules used by this pipeline. */
        Tools::Array<ShaderModule> vk_shaders;
        /* Array of create infos for the Shader stages of the pipeline. */
        Tools::Array<VkPipelineShaderStageCreateInfo> vk_shader_stages;
        /* Description of how Vulkan should pass a Vertex to the shaders. */
        VkVertexInputBindingDescription vk_vertex_input_binding;
        /* Array of structs that describe how to further handle a vertex from each buffer. */
        Tools::Array<VkVertexInputAttributeDescription> vk_vertex_input_attributes;
        /* Description of the vertex format given to the pipeline. */
        VkPipelineVertexInputStateCreateInfo vk_vertex_input_state;
        /* Description of what to do with the vertices passed to the pipeline. */
        VkPipelineInputAssemblyStateCreateInfo vk_vertex_assembly_state;
        /* Description of how each resulting viewport looks like. Note that using multiple requires a GPU feature. */
        Tools::Array<VkViewport> vk_viewports;
        /* Description of one or more scissor rectangles, cutting the image to the area of each rectangle. Note that using multiple requires a GPU feature. */
        Tools::Array<VkRect2D> vk_scissor_rects;
        /* Description of where to write in the resulting viewport. */
        VkPipelineViewportStateCreateInfo vk_viewport_state;
        /* Description of the rasterization stage of the pipeline. */
        VkPipelineRasterizationStateCreateInfo vk_rasterizer_state;
        /* Description of the multisampling stage of the pipeline. */
        VkPipelineMultisampleStateCreateInfo vk_multisample_state;
        /* Array of structs that describe how to combine the new colour with the one already there for each framebuffer. */
        Tools::Array<VkPipelineColorBlendAttachmentState> vk_color_attachments;
        /* Description of how to color blend each framebuffer, combined with additional options across framebuffers. */
        VkPipelineColorBlendStateCreateInfo vk_color_blend_state;
        /* Description of how the pipeline looks like, which can be used to change shader constants at runtime rather than having to re-compile them. */
        VkPipelineLayoutCreateInfo vk_pipeline_layout_info;


        /* Constructor for the GraphicsPipeline class, which only takes a device to create the pipeline on. */
        GraphicsPipeline(const Device& device);
        /* Copy constructor for the GraphicsPipeline class, which is deleted. */
        GraphicsPipeline(const GraphicsPipeline& other) = delete;
        /* Move constructor for the GraphicsPipeline class. */
        GraphicsPipeline(GraphicsPipeline&& other);
        /* Destructor for the GraphicsPipeline class. */
        virtual ~GraphicsPipeline();

        /* Virtual function to re-create the pipeline, based on the internally stored structs. Takes a render pass to render in this pipeline */
        virtual void regenerate(const RenderPass& render_pass) = 0;

        /* Explicitly returns the internal GraphicsPipeline object. */
        inline VkPipeline pipeline() const { return this->vk_pipeline; }
        /* Explicitly returns the internal VkPipelineLayout object. */
        inline VkPipelineLayout pipeline_layout() const { return this->vk_pipeline_layout; }
        /* Implicitly returns the internal GraphicsPipeline object, to cast this class to that. */
        inline operator VkPipeline() const { return this->vk_pipeline; }

    };
}

#endif
