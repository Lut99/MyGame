/* INSTANCE.cpp
 *   by Lut99
 *
 * Created:
 *   21/12/2020, 13:35:55
 * Last edited:
 *   21/12/2020, 13:35:55
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Wrapper for a Vulkan instance, that automatically handles allocation
 *   and deallocation.
**/

#define XSTR(x) STR(x)
#define STR(x) #x

#include <iostream>
#include <vector>

#include "Debug/Debug.hpp"
#include "Vulkan/Debugger.hpp"
#include "Vulkan/Instance.hpp"

using namespace std;
using namespace HelloVikingRoom;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** HELPER FUNCTIONS *****/
/* Populates a given VkApplicationInfo struct with this application's constants. */
void populate_application_info(VkApplicationInfo* app_info) {
    DENTER("populate_application_info");

    // Do the default initialization
    *app_info = {};
    app_info->sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    // Define the application's name
    app_info->pApplicationName = "HelloVikingRoom";

    // Define the application's version
    app_info->applicationVersion = VK_MAKE_VERSION(1, 0, 0);

    // Note if we're using a specific engine (we're not, though)
    app_info->pEngineName = "No Engine";

    // Also note the engine's version
    app_info->engineVersion = VK_MAKE_VERSION(1, 0, 0);

    // Finally, define the version of the API we're using
    app_info->apiVersion = VK_API_VERSION_1_0;

    DRETURN;
}





/***** INSTANCE CLASS *****/
/* Constructor for the Instance class, that takes a list of the extensions that we'll require from our Vulkan installation. Does not enable any validation layers or debuggers. */
Vulkan::Instance::Instance(const Tools::Array<const char*>& required_extensions) {
    DENTER("Vulkan::Instance::Instance");
    DLOG(info, "Creating Vulkan instance...");

    // Start with setting some specific constants for the Vulkan instance
    VkApplicationInfo app_info;
    populate_application_info(&app_info);

    // Next, we'll list which extensions to use
    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    // Tell vulkan which extenions to enable
    instance_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    instance_info.ppEnabledExtensionNames = required_extensions.data();
    // Tell Vulkan we enable no layers
    instance_info.enabledLayerCount = 0;
    instance_info.ppEnabledLayerNames = nullptr;

    // Finally create the struct itself
    if (vkCreateInstance(&instance_info, nullptr, &this->vk_instance) != VK_SUCCESS) {
        DLOG(fatal, "Could not create the Vulkan instance.");
    }

    // We're done
    DLEAVE;
}

/* Constructor for the Instance class, that takes a Debugger instance, a list of the extensions that we'll require from our Vulkan installation and a list of validation layers we'll want to enable. */
Vulkan::Instance::Instance(const Tools::Array<const char*>& required_extensions, const Tools::Array<const char*>& required_layers) {
    DENTER("Vulkan::Instance::Instance_DEBUG");
    DLOG(info, "Creating Vulkan instance...");

    // Start with setting some specific constants for the Vulkan instance
    VkApplicationInfo app_info;
    populate_application_info(&app_info);

    // Next, we'll list which extensions to use
    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    // Tell vulkan which extenions to enable
    instance_info.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
    instance_info.ppEnabledExtensionNames = required_extensions.data();
    // Tell Vulkan we want to enable our chosen layers
    instance_info.enabledLayerCount = static_cast<uint32_t>(required_layers.size());;
    instance_info.ppEnabledLayerNames = required_layers.data();
    // Then, provide a debugger create_info the the instance s.t. we can already begin getting debug messages
    VkDebugUtilsMessengerCreateInfoEXT debug_info;
    Debugger::populate_messenger_info(&debug_info);
    instance_info.pNext = (void*) &debug_info;

    // Finally create the struct itself
    if (vkCreateInstance(&instance_info, nullptr, &this->vk_instance) != VK_SUCCESS) {
        DLOG(fatal, "Could not create the Vulkan instance.");
    }

    // We're done
    DLEAVE;
}

/* Move constructor for the Instance class. */
Vulkan::Instance::Instance(Instance&& other) :
    vk_instance(other.vk_instance)
{
    // Set the other's instance to a nullptr
    other.vk_instance = nullptr;
}

/* Destructor for the Instance class. */
Vulkan::Instance::~Instance() {
    DENTER("Vulkan::Instance::~Instance()");
    DLOG(info, "Cleaning Vulkan instance...");

    // Only destroy the instance if it isn't a nullptr
    if (this->vk_instance != nullptr) {
        vkDestroyInstance(this->vk_instance, nullptr);
    }

    DLEAVE;
}
