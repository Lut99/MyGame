/* GRAPHICS PIPELINE.cpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 13:32:15
 * Last edited:
 *   13/01/2021, 13:32:15
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the GraphicsPipeline class, which forms the basis for all
 *   graphics pipelines. The individual pipelines are derived as their own
 *   classes.
**/

#include "Debug/Debug.hpp"
#include "GraphicsPipeline.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** GRAPHICSPIPELINE CLASS *****/
/* Constructor for the GraphicsPipeline class, which only takes a device to create the pipeline on. */
GraphicsPipeline::GraphicsPipeline(const Device& device) :
    vk_pipeline(nullptr),
    vk_pipeline_layout(nullptr),
    device(device),
    vk_vertex_input_binding({}),
    vk_vertex_input_state({}),
    vk_vertex_assembly_state({}),
    vk_viewport_state({}),
    vk_rasterizer_state({}),
    vk_multisample_state({}),
    vk_color_blend_state({}),
    vk_pipeline_layout_info({})
{}

/* Move constructor for the GraphicsPipeline class. */
GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) :
    vk_pipeline(other.vk_pipeline),
    vk_pipeline_layout(other.vk_pipeline_layout),
    device(other.device),
    vk_shaders(std::move(other.vk_shaders)),
    vk_shader_stages(other.vk_shader_stages),
    vk_vertex_input_binding(other.vk_vertex_input_binding),
    vk_vertex_input_attributes(other.vk_vertex_input_attributes),
    vk_vertex_input_state(other.vk_vertex_input_state),
    vk_vertex_assembly_state(other.vk_vertex_assembly_state),
    vk_viewports(other.vk_viewports),
    vk_scissor_rects(other.vk_scissor_rects),
    vk_viewport_state(other.vk_viewport_state),
    vk_rasterizer_state(other.vk_rasterizer_state),
    vk_multisample_state(other.vk_multisample_state),
    vk_color_attachments(other.vk_color_attachments),
    vk_color_blend_state(other.vk_color_blend_state),
    vk_pipeline_layout_info(other.vk_pipeline_layout_info)
{
    // Set the pipeline itself to nullptr to avoid destroying it
    other.vk_pipeline = nullptr;
    other.vk_pipeline_layout = nullptr;
}

/* Destructor for the GraphicsPipeline class. */
GraphicsPipeline::~GraphicsPipeline() {
    DENTER("Vulkan::GraphicsPipeline::~GraphicsPipeline");

    if (this->vk_pipeline != nullptr) {
        vkDestroyPipeline(this->device, this->vk_pipeline, nullptr);
    }
    if (this->vk_pipeline_layout != nullptr) {
        vkDestroyPipelineLayout(this->device, this->vk_pipeline_layout, nullptr);
    }

    DLEAVE;
}
