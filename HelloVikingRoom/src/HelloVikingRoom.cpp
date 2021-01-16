/* HELLO VIKING ROOM.cpp
 *   by Lut99
 *
 * Created:
 *   19/12/2020, 16:23:17
 * Last edited:
 *   1/14/2021, 12:49:38 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This is the more mature version of our implementation for the Vulkan
 *   Tutorial, where we (hopefully) make the code a lot more structured by
 *   subdividing things in classes in their own files. Additionally, we
 *   move past the triangle here, to a square and then to a sample
 *   3D-model, which is a Viking Room.
**/

#include <vulkan/vulkan.h>
#include <iostream>
#include <exception>
#include <algorithm>
#include <chrono>

#define GLM_FORCE_RADIANS
#include "glm/gtc/matrix_transform.hpp"
#include "Vertices/Vertex.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/Debugger.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Framebuffer.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/DescriptorSetLayout.hpp"
#include "Vulkan/DescriptorPool.hpp"
#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Fence.hpp"
#include "Vulkan/RenderPasses/SquarePass.hpp"
#include "Vulkan/GraphicsPipelines/SquarePipeline.hpp"
#include "Application/MainWindow.hpp"
#include "Tools/Array.hpp"
#include "Debug/Debug.hpp"

using namespace std;
using namespace HelloVikingRoom;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** STRUCTS *****/
/* The UniformBufferObject is used to pass transformation matrices to shaders. */
struct UniformBufferObject {
    /* The model matrix, which moves an object from model space (where the model's at 0, 0, 0) to world space. I.e., encodes the position of our model in the world. */
    glm::mat4 model;
    /* The view matrix, i.e., the camera. In mathmatical terms: translates all models in world space to camera space, where the camera is at (0, 0, 0). */
    glm::mat4 view;
    /* The projection matrix transforms the camera space to homogeneous space, which translates the normally-skewed camera box (trapezium-like) to a square so it's much easier to run the shaders. */
    glm::mat4 proj;
};





/***** CONSTANTS *****/
/* List of device extensions that we want to be enabled. */
const Array<const char*> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
/* List of validation layers that we want to be enabled. */
const Array<const char*> required_layers = {
    "VK_LAYER_KHRONOS_validation"
};

/* List of the vertices used for drawing the square. */
const Array<Vertex> vertices = {
    Vertex(glm::vec2(-0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)),
    Vertex(glm::vec2(0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)),
    Vertex(glm::vec2(0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)),
    Vertex(glm::vec2(-0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f))
};
/* Index buffer for the vertices. */
const Array<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
};





/***** HELPER FUNCTIONS *****/
/* Gets all global extensions as an Array. */
Array<const char*> get_global_extensions() {
    DENTER("get_global_extensions");
    DLOG(info, "Getting global extensions...");

    // Get the list of GLFW extensions
    uint32_t n_glfw_extensions = 0;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&n_glfw_extensions);

    // Put them in an array
    Array<const char*> result(glfw_extensions, n_glfw_extensions);

    #ifndef NDEBUG
    // We'll add the extension for the debugger
    result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

    // Done
    DRETURN result;
}

/* Checks if the given array of global extensions are supported by the current Vulkan installation. */
void verify_global_extensions(const Array<const char*>& to_verify) {
    DENTER("verify_global_extensions");
    DLOG(info, "Verifying if global extensions are supported...");

    // Get a list of all supported extensions on this system
    uint32_t n_existing_extensions = 0;
    if (vkEnumerateInstanceExtensionProperties(nullptr, &n_existing_extensions, nullptr) != VK_SUCCESS) {
        DLOG(fatal, "Could not get number of supported extensions.");
    }
    Array<VkExtensionProperties> existing_extensions(n_existing_extensions);
    if (vkEnumerateInstanceExtensionProperties(nullptr, &n_existing_extensions, existing_extensions.wdata(n_existing_extensions)) != VK_SUCCESS) {
        DLOG(fatal, "Could not get list of supported extensions.");
    }

    // Loop to find any missing extensions
    DINDENT;
    bool errored = false;
    for (size_t i = 0; i < to_verify.size(); i++) {
        // Check if this extension is in the list
        bool found = false;
        for (size_t j = 0; j < existing_extensions.capacity(); j++) {
            if (strcmp(to_verify[i], existing_extensions[j].extensionName) == 0) {
                // It exists
                found = true;
                break;
            }
        }
        if (!found) {
            DLOG(warning, std::string("Extension '") + to_verify[i] + "' is not supported");
            errored = true;
        }
    }
    DDEDENT;
    if (errored) {
        DLOG(fatal, "Missing required extensions; cannot continue.");
    }

    // Done
    DRETURN;
}

/* Checks if all the desired layers are present, and returns a list with layers that are. */
Array<const char*> trim_layers(const Array<const char*>& to_trim) {
    DENTER("trim_layers");
    DLOG(info, "Verifying if desired validation layers are supported...");

    // Get a list of all supported layers on this system
    uint32_t n_existing_layers = 0;
    if (vkEnumerateInstanceLayerProperties(&n_existing_layers, nullptr) != VK_SUCCESS) {
        DLOG(fatal, "Could not get number of supported layers.");
    }
    Array<VkLayerProperties> existing_layers(n_existing_layers);
    if (vkEnumerateInstanceLayerProperties(&n_existing_layers, existing_layers.wdata(n_existing_layers)) != VK_SUCCESS) {
        DLOG(fatal, "Could not get list of supported layers.");
    }

    // Loop to find any missing extensions
    Array<const char*> supported_layers(n_existing_layers);
    DINDENT;
    for (size_t i = 0; i < to_trim.size(); i++) {
        // Check if this extension is in the list
        bool found = false;
        for (size_t j = 0; j < existing_layers.capacity(); j++) {
            if (strcmp(to_trim[i], existing_layers[j].layerName) == 0) {
                // It exists
                supported_layers.push_back(existing_layers[j].layerName);
                found = true;
                break;
            }
        }
        if (!found) {
            DLOG(warning, std::string("Layer '") + to_trim[i] + "' is not supported");
        }
    }
    DDEDENT;

    // Decrease the size of the supported layers
    supported_layers.reserve(supported_layers.size());

    // Done
    DRETURN supported_layers;
}

/* Records the command buffer for a single framebuffer. */
void record_command_buffer(
    Vulkan::CommandBuffer& command_buffer,
    const Vulkan::GraphicsPipeline& graphics_pipeline,
    const Vulkan::RenderPass& render_pass,
    const Vulkan::Swapchain& swapchain,
    const Vulkan::Framebuffer& framebuffer,
    const Vulkan::Buffer& vertex_buffer,
    const Vulkan::Buffer& index_buffer,
    const Vulkan::DescriptorSetRef& descriptor_set
) {
    DENTER("record_command_buffer");
    DLOG(info, "Recording command buffer...");

    // Begin recording
    command_buffer.begin();

    // First, we'll start a render pass
    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    // Specify which render pass to begin
    render_pass_info.renderPass = render_pass;
    // Tell it which framebuffer to use
    render_pass_info.framebuffer = framebuffer;
    // Next, we specify what area to write to (the entire area). This is purely for shaders, and note that this means that the frame will be written to & read from for each shader
    render_pass_info.renderArea.offset = { 0, 0 };
    render_pass_info.renderArea.extent = swapchain.extent();
    // Now we'll specify which color to use when clearing the buffer (which we do when loading it) - it'll be fully black
    VkClearValue clear_color = { 0.0f, 0.0f, 0.0f, 1.0f };
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_color;
    // Time to start recording it with these configs. The final parameter decides if we execute the parameters in the primary buffer itself (INLINE) or in a secondary buffer.
    vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    // Next, register the pipeline to use
    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline);

    // Now we'll bind the buffers so the GPU knows to use them
    VkBuffer vertex_buffers[] = { vertex_buffer };
    VkDeviceSize offsets[] = { 0 };
    // Note that the command can be used to bind more buffers at once, but we won't do that
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    // Also bind the index buffer, specifying its type
    vkCmdBindIndexBuffer(command_buffer, index_buffer, 0, VK_INDEX_TYPE_UINT16);

    // Before we draw, bind the uniform buffers via their descriptors
    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline.pipeline_layout(), 0, 1, &descriptor_set.descriptor_set(), 0, nullptr);

    // We have told it how to start and how to render - all we have to tell it is what to render
    // Here, we pass the following information:
    //   - The command buffer that should start drawing
    //   - How many vertices we'll draw (the size of the global buffer, of course)
    //   - We don't do instance rendering (whatever that may be), so we pass 1
    //   - The first index in the vertex buffer, i.e., the lowest value of gl_VertexIndex in the shaders
    //   - The first index of the instance buffer, i.e., the lowest value of gl_InstanceIndex in the shaders (not used)
    vkCmdDrawIndexed(command_buffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

    // Once it has been drawn, we can end the render pass
    vkCmdEndRenderPass(command_buffer);

    // End recording
    command_buffer.end();

    DRETURN;
}

/* Helper function that resizes the swapchain and classes that (indirectly) use it. */
void resize_swapchain(
    MainWindow& window,
    Vulkan::Device& device,
    Vulkan::Swapchain& swapchain,
    Vulkan::RenderPass& render_pass,
    Vulkan::GraphicsPipeline& graphics_pipeline,
    Array<Vulkan::Framebuffer>& framebuffers,
    Vulkan::CommandPool& command_pool,
    Array<Vulkan::CommandBuffer>& command_buffers,
    const Vulkan::Buffer& vertex_buffer,
    const Vulkan::Buffer& index_buffer,
    Array<Vulkan::Buffer>& uniform_buffers,
    Vulkan::DescriptorPool& descriptor_pool,
    const Vulkan::DescriptorSetLayout& descriptor_layout,
    Array<Vulkan::DescriptorSetRef>& descriptor_sets
) {
    DENTER("resize_swapchain");

    // Wait until we're not minimized, i.e., the size is something other than 0x0
    int window_width = 0, window_height = 0;
    glfwGetFramebufferSize(window, &window_width, &window_height);
    while (window_width == 0 || window_height == 0) {
        // Simply wait until the next event
        window.do_events();
        glfwGetFramebufferSize(window, &window_width, &window_height);
    }

    // Next, wait until the device is idle before we re-create half of it
    device.wait_idle();

    // Now, re-create the swapchain, render pass and graphics pipeline
    device.refresh_info(window);
    swapchain.resize(window);
    render_pass.resize(swapchain);
    graphics_pipeline.resize(swapchain, render_pass);

    // Re-create the unchanged part of the framebuffers and the command buffers
    framebuffers.reserve(swapchain.imageviews().size());
    command_buffers.reserve(swapchain.imageviews().size());
    size_t to_resize = std::min(framebuffers.size(), swapchain.imageviews().size());
    for (size_t i = 0; i < to_resize; i++) {
        framebuffers[i].resize(swapchain.imageviews()[i], swapchain, render_pass);
    }

    // Create new framebuffers, command buffers & uniform buffers if the new swapchain size is larger than before
    for (size_t i = framebuffers.size(); i < swapchain.imageviews().size(); i++) {
        framebuffers.push_back(
            Vulkan::Framebuffer(device, swapchain.imageviews()[i], swapchain, render_pass)
        );
        command_buffers.push_back(
            command_pool.get_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY)
        );
        uniform_buffers.push_back(Vulkan::Buffer(
            device,
            sizeof(UniformBufferObject),
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        ));
    }

    // Also update all the descriptor pool and all its sets
    descriptor_pool.resize(static_cast<uint32_t>(swapchain.images().size()), static_cast<uint32_t>(swapchain.images().size()));
    descriptor_sets = descriptor_pool.get_descriptor(static_cast<uint32_t>(swapchain.images().size()), descriptor_layout);
    for (size_t i = 0; i < descriptor_sets.size(); i++) {
        descriptor_sets[i].set(uniform_buffers[i]);
    }

    // Next, record all command buffers again with all the re-done structures
    for (size_t i = 0; i < command_buffers.size(); i++) {
        record_command_buffer(
            command_buffers[i],
            graphics_pipeline,
            render_pass,
            swapchain,
            framebuffers[i],
            vertex_buffer,
            index_buffer,
            descriptor_sets[i]
        );
    }

    // Finally, reset the window resize state and we're done
    window.reset_resized();
    DRETURN;
}

/* Helper function that computes the new transformation matrices s.t. the image will nicely rotate. */
void update_uniform_buffer(const MainWindow& window, Array<Vulkan::Buffer>& uniform_buffers, const Vulkan::Swapchain& swapchain, uint32_t image_index) {
    DENTER("update_uniform_buffer");

    // Use a static variable to keep track of the last time the function was called
    static std::chrono::high_resolution_clock::time_point last_update = std::chrono::high_resolution_clock::now();
    // Use another static to keep track of how far we rotated
    static float rotation_state = 0;

    // Mod that by the amount move
    if (window.left_pressed()) {
        // Compute the time that has passed since last call
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        // Add that to the rotation state
        rotation_state += std::chrono::duration_cast<std::chrono::duration<float>>(now - last_update).count();
    } else if (window.right_pressed()) {
        // Compute the time that has passed since last call
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        // Add that to the rotation state (inverted since we got the other way)
        rotation_state -= std::chrono::duration_cast<std::chrono::duration<float>>(now - last_update).count();
    }

    // Define the translation matrices
    UniformBufferObject translations{};
    // First we translate the model to world space; in this case, we rotate it over the Z-axis (last vec) by 90 degrees, depending on the amount of time passed
    // Since it's the first translation, we start with the unit matrix
    translations.model = glm::rotate(glm::mat4(1.0f), rotation_state * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // Next, we add the view matrix. It will be lookup straight at the square, but then above and away (2, 2, 2) from 45 degrees down. The up axis is here defined to be the Z-axis.
    translations.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    // Finally, the projection matrix, which has a field-of-view of 45 degrees and uses the swapchain to keep the aspect ratio equal to the window size.
    // The final two parameters are the near plane and the far plane; presumably the locations of the near and far 'square' of the camera trapezium
    translations.proj = glm::perspective(glm::radians(45.0f), (float) swapchain.extent().width / (float) swapchain.extent().height, 0.1f, 10.0f);
    // Don't forget to flip the Y-axis of the translation matrix (we flip the Y-scalar), though, as this library is for OpenGL and that uses an inverted Y-axis
    translations.proj[1][1] *= -1;

    // Next, we update all the uniform buffer for the current image
    uniform_buffers[image_index].set((void*) &translations, sizeof(UniformBufferObject));

    // Note that we updated this the last time
    last_update = std::chrono::high_resolution_clock::now();

    DRETURN;
}





/***** ENTRY POINT *****/
int main() {
    DSTART("main thread"); DENTER("main");
    DLOG(auxillary, "");
    DLOG(auxillary, "<<<<< HELLO VIKINGROOM >>>>>");
    DLOG(auxillary, "");

    // First, initialize the glfw library
    glfwInit();

    // Wrap all code in a try/catch to neatly handle the errors that our DEBUGGER may throw
    try {
        /***** STEP 1: Initialization *****/
        // Get all the extensions for our window library
        Array<const char*> global_extensions = get_global_extensions();
        // Check if we can use them
        verify_global_extensions(global_extensions);

        // Create a vulkan instance
        #ifndef NDEBUG
        // If debug is defined, then also check if the layers are supported
        Array<const char*> trimmed_layers = trim_layers(required_layers);
        Vulkan::Instance instance(global_extensions, trimmed_layers);
        #else
        // Just add the extensions, no validation layers
        Vulkan::Instance instance(global_extensions);
        #endif

        // Initialize the debugger
        Vulkan::Debugger debugger(instance);

        // Create a window with that instance
        MainWindow window(instance, "Hello Viking Room", 800, 600);

        // Create a Device instance
        Vulkan::Device device(instance, window.surface(), device_extensions);
        // Create the swapchain for that device
        Vulkan::Swapchain swapchain(window, device);

        // Create the descriptor layout to bind the uniform buffer for the transformation matrices
        Vulkan::DescriptorSetLayout descriptor_set_layout(device, VK_SHADER_STAGE_VERTEX_BIT);
        Array<VkDescriptorSetLayout> descriptor_set_layouts({ descriptor_set_layout });

        // Create our only render pass (for now), and use that to create a graphics pipeline
        Vulkan::RenderPasses::SquarePass render_pass(device, swapchain);
        Vulkan::GraphicsPipelines::SquarePipeline pipeline(device, swapchain, render_pass, descriptor_set_layouts);

        // Create the framebuffers
        Array<Vulkan::Framebuffer> framebuffers(swapchain.imageviews().size());
        for (size_t i = 0; i < swapchain.imageviews().size(); i++) {
            framebuffers.push_back(
                Vulkan::Framebuffer(device, swapchain.imageviews()[i], swapchain, render_pass)
            );
        }

        // Create the command pool for all graphics queues
        Vulkan::CommandPool command_pool(device, device.get_queue_info().graphics(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        // Create the vertex buffer
        Vulkan::Buffer vertex_buffer(device, sizeof(Vertex) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vertex_buffer.set_staging((void*) vertices.rdata(), sizeof(Vertex) * vertices.size(), command_pool);
        // Create the index buffer
        Vulkan::Buffer index_buffer(device, sizeof(uint16_t) * indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        index_buffer.set_staging((void*) indices.rdata(), sizeof(uint16_t) * indices.size(), command_pool);
        // Create the uniform buffers for the transformation matrices, one per frame in the framebuffers
        Array<Vulkan::Buffer> uniform_buffers(swapchain.imageviews().size());
        for (size_t i = 0; i < swapchain.imageviews().size(); i++) {
            uniform_buffers.push_back(Vulkan::Buffer(
                device,
                sizeof(UniformBufferObject),
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            ));
        }

        // Create the descriptor pool for the uniform buffers and get the sets from that
        Vulkan::DescriptorPool descriptor_pool(device, static_cast<uint32_t>(swapchain.images().size()), static_cast<uint32_t>(swapchain.images().size()));
        Array<Vulkan::DescriptorSetRef> descriptor_sets = descriptor_pool.get_descriptor(static_cast<uint32_t>(swapchain.images().size()), descriptor_set_layout);
        for (size_t i = 0; i < descriptor_sets.size(); i++) {
            descriptor_sets[i].set(uniform_buffers[i]);
        }

        // Create the command buffers for each frame in the swapchain
        Array<Vulkan::CommandBuffer> command_buffers = command_pool.get_buffer(framebuffers.size(), VK_COMMAND_BUFFER_LEVEL_PRIMARY);
        for (size_t i = 0; i < command_buffers.size(); i++) {
            // Call the helper function to actually record it
            record_command_buffer(
                command_buffers[i],
                pipeline,
                render_pass,
                swapchain,
                framebuffers[i],
                vertex_buffer,
                index_buffer,
                descriptor_sets[i]
            );
        }

        // Finally, prepare the synchronization objects
        // Signals if an image is ready for drawing
        Array<Vulkan::Semaphore> image_ready_semaphores(framebuffers.size());
        // Signals if an image is done with being rendered to
        Array<Vulkan::Semaphore> image_rendered_semaphores(framebuffers.size());
        // Fence that determines if a swapchain framebuffer is available or not
        Array<std::shared_ptr<Vulkan::Fence>> frame_in_flight_fences(framebuffers.size());
        // Fence that determines if a swapchain image is available or not. Is the exact copy of a frame_in_flight_fence.
        Array<std::shared_ptr<Vulkan::Fence>> image_in_flight_fences(framebuffers.size());
        for (size_t i = 0; i < framebuffers.size(); i++) {
            image_ready_semaphores.push_back(Vulkan::Semaphore(device));
            image_rendered_semaphores.push_back(Vulkan::Semaphore(device));
            frame_in_flight_fences.push_back(std::shared_ptr<Vulkan::Fence>(new Vulkan::Fence(device)));
            image_in_flight_fences.push_back(nullptr);
        }



        /***** STEP 2: MAIN LOOP *****/
        DLOG(info, "Running main loop...");
        size_t current_frame = 0;
        while (!window.done()) {
            // Handle any window events (like resizing, ending, etc)
            window.do_events();



            /***** STEP 1: GETTING AN IMAGE *****/

            // Wait until our current frame is done with the previous render pass
            frame_in_flight_fences[current_frame]->wait();

            // Next, we'll get a "new" image from the swapchain. We pass it an image_ready semaphore to keep track of when it's ready, and this is also where we handle window resizes
            uint32_t image_index;
            VkResult get_image_result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_ready_semaphores[current_frame], VK_NULL_HANDLE, &image_index);
            if (get_image_result == VK_ERROR_OUT_OF_DATE_KHR || get_image_result == VK_SUBOPTIMAL_KHR || window.resized()) {
                // The window changed size
                resize_swapchain(
                    window,
                    device,
                    swapchain,
                    render_pass,
                    pipeline,
                    framebuffers,
                    command_pool,
                    command_buffers,
                    vertex_buffer,
                    index_buffer,
                    uniform_buffers,
                    descriptor_pool,
                    descriptor_set_layout,
                    descriptor_sets
                );
                image_ready_semaphores[current_frame].reset();
                continue;
            } else if (get_image_result != VK_SUCCESS) {
                // We failed getting an image
                DLOG(fatal, "Failed to get image from swapchain.");
            }



            /***** STEP 2: UPDATING THE TRANSFORMATION MATRICES *****/

            // Wait until the image is not in use either, and not just the frame. Note that this will be skipped if the image isn't retrieved at least once yet
            if (image_in_flight_fences[image_index] != nullptr) {
                vkWaitForFences(device, 1, &image_in_flight_fences[image_index]->fence(), VK_TRUE, UINT64_MAX);
            }
            // Update the fence to the equivalent frame fence
            image_in_flight_fences[image_index] = frame_in_flight_fences[current_frame];

            // Call our update function
            update_uniform_buffer(window, uniform_buffers, swapchain, image_index);



            /***** STEP 3: SUBMITTING THE RENDER COMMAND BUFFER *****/

            // Next, we'll submit the correct command buffer to draw the triangle
            VkSubmitInfo submit_info{};
            submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            // Pass it the semaphores to wait for before starting the command buffer, telling it at which stage in the pipeline to do the waiting
            VkSemaphore wait_semaphores[] = { image_ready_semaphores[current_frame] };
            VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submit_info.waitSemaphoreCount = 1;
            submit_info.pWaitSemaphores = wait_semaphores;
            submit_info.pWaitDstStageMask = wait_stages;
            // Next, define the command buffer to use
            submit_info.commandBufferCount = 1;
            submit_info.pCommandBuffers = &(command_buffers[image_index].command_buffer());
            // Now we tell it which semaphore to signal once the command buffer is actually done processing
            VkSemaphore signal_semaphores[] = { image_rendered_semaphores[current_frame] };
            submit_info.signalSemaphoreCount = 1;
            submit_info.pSignalSemaphores = signal_semaphores;
            // Reset this frame's fence, so that we signal it's in use again
            vkResetFences(device, 1, &frame_in_flight_fences[current_frame]->fence());

            // Now that's done, submit the command buffer to the queue!
            // Note that we use the fence to be able to say if this frame was done or not, and that we may schedule more than one command buffers at once
            if (vkQueueSubmit(device.graphics_queue(), 1, &submit_info, *frame_in_flight_fences[current_frame]) != VK_SUCCESS) {
                DLOG(fatal, "Could not submit command buffer to the graphics queue.");
            }



            /***** STEP 4: PRESENTING THE FRAME *****/

            // With the rendering process scheduled, it's now time to define what happens when that's done
            VkPresentInfoKHR present_info{};
            present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            // Let it specify which semaphore to wait for until beginning
            present_info.waitSemaphoreCount = 1;
            present_info.pWaitSemaphores = signal_semaphores;
            // Next, define which swapchain we'll present which image to
            VkSwapchainKHR swap_chains[] = { swapchain };
            present_info.swapchainCount = 1;
            present_info.pSwapchains = swap_chains;
            present_info.pImageIndices = &image_index;
            // If we're writing to multiple swapchains, we can use this pointer to let us get a list of results for each of them
            present_info.pResults = nullptr;

            // Let's present it!
            VkResult present_result = vkQueuePresentKHR(device.presentation_queue(), &present_info);
            if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || window.resized()) {
                // The window changed size
                resize_swapchain(
                    window,
                    device,
                    swapchain,
                    render_pass,
                    pipeline,
                    framebuffers,
                    command_pool,
                    command_buffers,
                    vertex_buffer,
                    index_buffer,
                    uniform_buffers,
                    descriptor_pool,
                    descriptor_set_layout,
                    descriptor_sets
                );
                image_ready_semaphores[current_frame].reset();
                continue;
            } else if (present_result != VK_SUCCESS) {
                // We failed
                DLOG(info, "Could not submit resulting image to the presentation queue");
            }



            /***** STEP 5: MOVE TO NEXT FRAME *****/

            // Once done, update the frame to the next loop
            if (++current_frame >= 3) { current_frame = 0; }
        }

        // Once done with the main loop, be sure to wait until the device is ready as well
        device.wait_idle();

    } catch (std::exception&) {
        // Destroy the GLFW library
        glfwTerminate();

        DRETURN EXIT_FAILURE;
    }

    // Destroy the GLFW library
    glfwTerminate();

    DLOG(auxillary, "");
    DLOG(auxillary, "Done.");
    DLOG(auxillary, "");

    DRETURN EXIT_SUCCESS;
}
