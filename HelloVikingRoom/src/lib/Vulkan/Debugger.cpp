/* DEBUGGER.cpp
 *   by Lut99
 *
 * Created:
 *   21/12/2020, 16:30:45
 * Last edited:
 *   21/12/2020, 16:30:45
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains code that creates the Vulkan debugger information.
**/

#include "Vulkan/Debugger.hpp"

using namespace std;
using namespace HelloVikingRoom;
using namespace Debug::SeverityValues;


/***** DEBUGGER CLASS *****/
/* Constructor for the Debugger class, which takes an Instance class to bind the debugger to. */
Vulkan::Debugger::Debugger(const Instance& instance) :
    instance(instance)
{
    DENTER("Vulkan::Debugger::Debugger");
    DLOG(info, "Setting up the Vulkan debug messenger...");

    // Start by loading the pointers to the functions
    DLOG(auxillary, "Loading Vulkan extensions function 'vkCreateDebugUtilsMessengerEXT'...");
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (vkCreateDebugUtilsMessengerEXT == nullptr) {
        DLOG(fatal, "Could not load extension function 'vkCreateDebugUtilsMessengerEXT'");
    }

    // Also already bind the destroy function
    DLOG(auxillary, "Loading Vulkan extensions function 'vkDestroyDebugUtilsMessengerEXT'...");
    this->vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (this->vkDestroyDebugUtilsMessengerEXT == nullptr) {
        DLOG(fatal, "Could not load extension function 'vkDestroyDebugUtilsMessengerEXT'");
    }

    // We'll create the messenger by using our populate() call
    VkDebugUtilsMessengerCreateInfoEXT debug_info;
    this->populate_messenger_info(&debug_info);
    if (vkCreateDebugUtilsMessengerEXT(this->instance, &debug_info, nullptr, &this->vk_debugger) != VK_SUCCESS) {
        DLOG(fatal, "Could not setup Vulkan's debug messenger.");
    }

    // And that's it!
    DLEAVE;
}

/* Move constructor for the Debugger class. */
Vulkan::Debugger::Debugger(Debugger&& other) :
    vk_debugger(other.vk_debugger),
    instance(other.instance)
{
    // Set the other pointer to nullptr s.t. it doesn't destroy the debugger
    other.vk_debugger = nullptr;
}

/* Destructor for the Debugger class. */
Vulkan::Debugger::~Debugger() {
    DENTER("Vulkan::Debugger::~Debugger()");
    DLOG(info, "Cleaning Vulkan debug messenger...");

    // Only destroy if not a nullptr
    if (this->vk_debugger != nullptr) {
        this->vkDestroyDebugUtilsMessengerEXT(this->instance, this->vk_debugger, nullptr);
    }

    DLEAVE;
}



/* Static callback function for Vulkan to call whenever it wants to log something. */
VKAPI_ATTR VkBool32 VKAPI_CALL Vulkan::Debugger::vk_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* user_data
) {
    // Determine the logging severity
    Debug::Severity severity;
    switch(message_severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severity = info;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severity = warning;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severity = nonfatal;
            break;
        default:
            severity = (Debug::Severity) -1;
            break;
    }

    // Do not use the message type for now
    (void) message_type;

    // Log the message with the correct severity
    Debug::Debugger* debugger = (Debug::Debugger*) user_data;
    debugger->log(severity, std::string("[\033[1mVULKAN\033[0m] ") + pCallbackData->pMessage);
    return VK_FALSE;
}



/* Populates a given create_info struct for the VkDebugUtilsMessengerEXT class. */
void Vulkan::Debugger::populate_messenger_info(VkDebugUtilsMessengerCreateInfoEXT* debug_info) {
    DENTER("Vulkan::Debugger::populate_messenger_info");

    // Do the default initialization
    *debug_info = {};
    debug_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

    // Define the severities we'll get called for
    debug_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    // Define ->he message types we'll get called for
    debug_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    // Mark what our callback is (the one that utilizes our own debugger)
    debug_info->pfnUserCallback = Vulkan::Debugger::vk_callback;

    // Provide our own Debugger as user data
    debug_info->pUserData = (void*) &Debug::debugger;

    DRETURN;
}
