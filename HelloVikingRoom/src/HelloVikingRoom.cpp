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
#include <array>

#include "Vertices/Vertex.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/Debugger.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Framebuffer.hpp"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/RenderPasses/SquarePass.hpp"
#include "Vulkan/GraphicsPipelines/SquarePipeline.hpp"
#include "Application/MainWindow.hpp"
#include "Tools/Array.hpp"
#include "Debug/Debug.hpp"

using namespace std;
using namespace HelloVikingRoom;
using namespace Tools;
using namespace Debug::SeverityValues;


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
    Vertex(glm::vec2(0.0f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)),
    Vertex(glm::vec2(0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)),
    Vertex(glm::vec2(-0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f))
};
/* Index buffer for the vertices. */
const Array<uint16_t> indices = {
    0, 1, 2
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

        // Create our only render pass (for now), and use that to create a graphics pipeline
        Vulkan::RenderPasses::SquarePass render_pass(device, swapchain);
        Vulkan::GraphicsPipelines::SquarePipeline pipeline(device, swapchain, render_pass);

        // Create the framebuffers
        Array<Vulkan::Framebuffer> framebuffers(swapchain.imageviews().size());
        for (size_t i = 0; i < swapchain.imageviews().size(); i++) {
            framebuffers.push_back(
                Vulkan::Framebuffer(device, swapchain.imageviews()[i], swapchain, render_pass)
            );
        }

        // Create the command pool for all graphics queues
        Vulkan::CommandPool command_pool(device, device.get_queue_info().graphics());

        // Create the vertex buffer
        Vulkan::Buffer vertex_buffer(device, sizeof(Vertex) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        vertex_buffer.set_staging((void*) vertices.rdata(), sizeof(Vertex) * vertices.size(), command_pool);
        // Create the index buffer
        Vulkan::Buffer index_buffer(device, sizeof(uint16_t) * indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        index_buffer.set_staging((void*) indices.rdata(), sizeof(uint16_t) * indices.size(), command_pool);



        /***** STEP 2: MAIN LOOP *****/
        DLOG(info, "Running main loop...");
        while (!window.done()) {
            window.do_events();
        }



    } catch (std::exception&) {
        DRETURN EXIT_FAILURE;
    }

    // Destroy the GLFW library
    glfwTerminate();

    DLOG(auxillary, "");
    DLOG(auxillary, "Done.");
    DLOG(auxillary, "");

    DRETURN EXIT_SUCCESS;
}
