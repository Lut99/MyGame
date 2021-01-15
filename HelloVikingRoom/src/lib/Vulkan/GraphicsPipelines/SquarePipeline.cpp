/* SQUARE PIPELINE.cpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 14:11:43
 * Last edited:
 *   13/01/2021, 14:11:43
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the SquarePipeline class, which is a specialized pipeline for
 *   rendering the simple square from HelloTriangle. Derives from the
 *   GraphicsPipeline class.
**/

#include "Vertices/Vertex.hpp"
#include "Debug/Debug.hpp"
#include "SquarePipeline.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan::GraphicsPipelines;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** SQUAREPIPELINE CLASS *****/
/* Constructor for the SquarePipeline class, which takes the device to create the pipeline on, a swapchain to deduce the image format from and a render pass to render in the pipeline. */
SquarePipeline::SquarePipeline(const Device& device, const Swapchain& swapchain, const RenderPass& render_pass) :
    GraphicsPipeline(device)
{
    DENTER("Vulkan::GraphicsPipelines::SquarePipeline::SquarePipeline");
    DLOG(info, "Creating Vulkan SquarePipeline graphics pipeline...");

    /* Create the shaders (i.e., configurable stages) first. */
    // Start by loading the required shader modules
    this->vk_shaders = {
        ShaderModule(device, "./vert.spv"),
        ShaderModule(device, "./frag.spv")
    };
    // Create the VkPipelineShaderStage objects for each shader
    for (size_t i = 0; i < this->vk_shaders.size(); i++) {
        // Create the create info struct for this shader
        this->vk_shader_stages.push_back({});
        this->vk_shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        // Pass the struct's type
        this->vk_shader_stages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        // Mark that this stage info is for the vertex stage
        this->vk_shader_stages[i].stage = i == 0 ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        // Pass the module that we'll be using - different depending on which shader we loaded
        this->vk_shader_stages[i].module = this->vk_shaders[i];
        // Tell it which function to use as entrypoint (neat!)
        this->vk_shader_stages[i].pName = "main";
        // Optionally, we can tell it to set specific constants before we are going to compile it further (we won't now, though)
        this->vk_shader_stages[i].pSpecializationInfo = nullptr;
    }



    /* Define the fixed stages second. */
    // First, we'll define the vertex input for our pipeline; get the binding description & attributes first, then create the struct
    this->vk_vertex_input_binding = Vertex::getBindingDescription();
    this->vk_vertex_input_attributes = Vertex::getAttributeDescriptions();
    this->vk_vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // Tell it that we  have 1 description for the input data
    this->vk_vertex_input_state.vertexBindingDescriptionCount = 1;
    // Tell it again by providing the data
    this->vk_vertex_input_state.pVertexBindingDescriptions = &this->vk_vertex_input_binding;
    // Tell it that we have 2 attributes for the input data
    this->vk_vertex_input_state.vertexAttributeDescriptionCount = static_cast<uint32_t>(this->vk_vertex_input_attributes.size());
    // Tell it again by providing the data
    this->vk_vertex_input_state.pVertexAttributeDescriptions = this->vk_vertex_input_attributes.rdata();

    // Next, we'll tell the pipeline what to do with the vertices we give it. We'll be using meshes, so we'll tell it to draw triangles between every set of three points given
    this->vk_vertex_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    // This is where we tell it to treat if as a list of triangles
    this->vk_vertex_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Here we tell it not to restart, which I think means that we can't change topology halfway through
    this->vk_vertex_assembly_state.primitiveRestartEnable = VK_FALSE;

    // Next, we define where to write to in the viewport. Note that we pretty much always write to the entire viewport.
    this->vk_viewports.push_back({});
    // We tell it to start at the left...
    this->vk_viewports[0].x = 0.0f;
    // ...top corner...
    this->vk_viewports[0].y = 0.0f;
    // Then, set the minimum depth to the standard value of 0.0
    this->vk_viewports[0].minDepth = 0.0f;
    // And the maximum depth to the standard value of 1.0
    this->vk_viewports[0].maxDepth = 1.0f;

    // While the viewport is used to scale the the rectangle, we can use a scissor rectangle to cut it. This, too, we'll set to the size of the viewport
    this->vk_scissor_rects.push_back({});
    // No type either, instead we immediately set the xy
    this->vk_scissor_rects[0].offset = { 0, 0 };

    // We'll now combine the viewport and scissor into a viewport state. Note that multiple viewports can be used on some cards, but we'll use just one
    this->vk_viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    // Tell it how many viewports we're using
    this->vk_viewport_state.viewportCount = 1;
    // Give it an "array" of viewports to use
    this->vk_viewport_state.pViewports = &this->vk_viewports[0];
    // Similarly, tell it to use only one scissor rect
    this->vk_viewport_state.scissorCount = 1;
    // Give it an "array" of scissor rects to use
    this->vk_viewport_state.pScissors = &this->vk_scissor_rects[0];

    // Next, we'll initialize the rasterizer, which can do some wizard graphics stuff I don't know yet
    this->vk_rasterizer_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    // Do not tell it to clamp depths (whatever that may be)
    this->vk_rasterizer_state.depthClampEnable = VK_FALSE;
    // If this value is set to true, then the rasterizer is the last step of the pipeline as it discards EVERYTHING
    this->vk_rasterizer_state.rasterizerDiscardEnable = VK_FALSE;
    // Tells the rasterizer what to do with a polygon. Can either fill it, draw the lines or just to points of it. Note that non-fill requires a GPU feature to be enabled.
    this->vk_rasterizer_state.polygonMode = VK_POLYGON_MODE_FILL;
    // We tell the rasterizer how thicc it's lines need to be. >= 1.0? Needs a GPU feature enabled!
    this->vk_rasterizer_state.lineWidth = 1.0f;
    // Next, tell the rasterizer how to 'cull'; i.e., whether it should remove no sides of all objects, the front side, the backside or no side
    this->vk_rasterizer_state.cullMode = VK_CULL_MODE_BACK_BIT;
    // Determines how the rasterizer knows if it's a front or back. Can be clockwise or counterclockwise (however that may work)
    this->vk_rasterizer_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
    // We won't do anything depth-related for now, so the next four values are just set to 0 and false etc
    this->vk_rasterizer_state.depthBiasEnable = VK_FALSE;
    this->vk_rasterizer_state.depthBiasConstantFactor = 0.0f; // Optional
    this->vk_rasterizer_state.depthBiasClamp = 0.0f; // Optional
    this->vk_rasterizer_state.depthBiasSlopeFactor = 0.0f; // Optional

    // Next, we'll look at multisampling; for now, we won't do that yet (since we only have a single triangle anyway and a very simple scene)
    this->vk_multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    // Tell it not to multisample shadows?
    this->vk_multisample_state.sampleShadingEnable = VK_FALSE;
    // We'll only sample once
    this->vk_multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    // Tell it to sample a minimum of 1 times (optional)
    this->vk_multisample_state.minSampleShading = 1.0f;
    // The mask of sampling is nothing either (optional)
    this->vk_multisample_state.pSampleMask = nullptr;
    // Tell it not to enable some alpha-related stuff
    this->vk_multisample_state.alphaToCoverageEnable = VK_FALSE;
    // Tell it not to enable some alpha-related stuff
    this->vk_multisample_state.alphaToOneEnable = VK_FALSE;

    // Next, we'll talk colour blending; how should the pipeline combine the result of the fragment shader with what is already in the buffer?
    this->vk_color_attachments.push_back({});
    // Don't tell it it's type but to tell it which colors to write
    this->vk_color_attachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // Tell it to blend (true) or overwrite with the new value (false). Let's choose true, to blend alpha channels
    this->vk_color_attachments[0].blendEnable = VK_TRUE;
    // The factor to blend with (i.e., ratio of color used) of the source; we'll use the alpha channel for that
    this->vk_color_attachments[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    // The factor to blend with the destination color (the one already there) is precisely the inverse
    this->vk_color_attachments[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    // We tell the pipeline to combine the two colours using a simple add-operation (since they're inverse anyway)
    this->vk_color_attachments[0].colorBlendOp = VK_BLEND_OP_ADD;
    // Next, we copy the alpha color of the source colour (i.e., the one given by the fragment shader)
    this->vk_color_attachments[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    // That means we won't use the destination alpha at all
    this->vk_color_attachments[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    // Finally, how do we merge the alpha colours?
    this->vk_color_attachments[0].alphaBlendOp = VK_BLEND_OP_ADD;

    // The second struct for color blending is used to group all the states for each framebuffer
    this->vk_color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    // We tell the pipeline not to blend at all using logical operators between the buffers. Setting this to true would have overridden the per-buffer settings
    this->vk_color_blend_state.logicOpEnable = VK_FALSE;
    // It doesn't matter which logic operation we pass to the struct, since it's disabled
    this->vk_color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    // What is important is passing information per frame buffer (of which we have one)
    this->vk_color_blend_state.attachmentCount = static_cast<uint32_t>(this->vk_color_attachments.size());
    this->vk_color_blend_state.pAttachments = this->vk_color_attachments.rdata();
    // The next few constants are also irrelevent now no logic compying is done
    this->vk_color_blend_state.blendConstants[0] = 0.0f;
    this->vk_color_blend_state.blendConstants[1] = 0.0f;
    this->vk_color_blend_state.blendConstants[2] = 0.0f;
    this->vk_color_blend_state.blendConstants[3] = 0.0f;

    // Now it's time to look at the layout of our pipeline. This is used to be able to change shader constants at draw time rather than compile time
    this->vk_pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    // For now, we just set everything to empty, since we don't do any fancy dynamic stuff yet
    this->vk_pipeline_layout_info.setLayoutCount = 0;
    this->vk_pipeline_layout_info.pSetLayouts = nullptr;
    this->vk_pipeline_layout_info.pushConstantRangeCount = 0;
    this->vk_pipeline_layout_info.pPushConstantRanges = nullptr;

    // Actually create the VkPipelineLayout struct now
    if (vkCreatePipelineLayout(this->device, &this->vk_pipeline_layout_info, nullptr, &this->vk_pipeline_layout) != VK_SUCCESS) {
        // Do the error
        DLOG(fatal, "Could not create the pipeline layout for the SquarePipeline.");
    }



    /* Finally, leave the pipeline creation to our regenerate() function. */
    this->resize(swapchain, render_pass);

    DLEAVE;
}

/* Move constructor for the SquarePipeline class. */
SquarePipeline::SquarePipeline(SquarePipeline&& other) :
    GraphicsPipeline(std::move(other))
{}

/* Destructor for the SquarePipeline class. */
SquarePipeline::~SquarePipeline() {
    DENTER("Vulkan::GraphicsPipelines::SquarePipeline::~SquarePipeline");
    DLOG(info, "Cleaning Vulkan SquarePipeline graphics pipeline...");
    DLEAVE;
}



/* Virtual function to re-create the pipeline, based on the internally stored structs. Takes a render pass to render in this pipeline */
void SquarePipeline::resize(const Swapchain& swapchain, const RenderPass& render_pass) {
    DENTER("Vulkan::GraphicsPipelines::SquarePipeline::resize");
    
    // Remove the old graphics pipeline if it exists
    if (this->vk_pipeline != nullptr) {
        vkDestroyPipeline(this->device, this->vk_pipeline, nullptr);
    }

    // Update the relevant create structs to incorporate the swapchain changes
    this->vk_viewports[0].width = (float) swapchain.extent().width;
    this->vk_viewports[0].height = (float) swapchain.extent().height;
    this->vk_scissor_rects[0].extent = swapchain.extent();

    // We start, as always, by defining the struct
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    // First, we define the shaders we have
    pipeline_info.stageCount = static_cast<uint32_t>(this->vk_shader_stages.size());
    pipeline_info.pStages = this->vk_shader_stages.rdata();
    // Next, we put in all the fixed-stage structs, like..
    // ...the vertex input (how the vertices look like)
    pipeline_info.pVertexInputState = &this->vk_vertex_input_state;
    // ...the input assembly state (what to do with the vertices)
    pipeline_info.pInputAssemblyState = &this->vk_vertex_assembly_state;
    // ...how the viewport looks like
    pipeline_info.pViewportState = &this->vk_viewport_state;
    // ...how to rasterize the vertices
    pipeline_info.pRasterizationState = &this->vk_rasterizer_state;
    // ...if we do multisampling
    pipeline_info.pMultisampleState = &this->vk_multisample_state;
    // ...if we do something with depths and stencils (which we don't)
    pipeline_info.pDepthStencilState = nullptr;
    // ...how to go about merging the new frame with an old one
    pipeline_info.pColorBlendState = &this->vk_color_blend_state;
    // ...and if there are any dynamic states (which there aren't)
    pipeline_info.pDynamicState = nullptr;
    // Next, we define the layout of the pipeline (not a pointer)
    pipeline_info.layout = this->vk_pipeline_layout;
    // Now, we define the renderpass we'll use
    pipeline_info.renderPass = render_pass;
    // And which of the subpasses to start with
    pipeline_info.subpass = 0;
    // There is no 'base' pipeline, which could optionally be used to make creation of pipelines which are mostly the same easier
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    // Similarly, we have no index for the base pipeline since there is none
    pipeline_info.basePipelineIndex = -1;

    // Create the pipeline struct!
    // Note that it can be used to create multiple pipelines at once. The null handle can be used to give a cache, to store pipelines in between program executions (neat!)
    if (vkCreateGraphicsPipelines(this->device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->vk_pipeline) != VK_SUCCESS) {
        DLOG(fatal, "Could not create graphics pipeline.");
    }

    DRETURN;
}
