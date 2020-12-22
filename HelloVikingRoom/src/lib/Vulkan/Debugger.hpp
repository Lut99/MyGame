/* DEBUGGER.hpp
 *   by Lut99
 *
 * Created:
 *   21/12/2020, 16:22:22
 * Last edited:
 *   21/12/2020, 16:22:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains code that creates the Vulkan debugger information.
**/

#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <vulkan/vulkan.h>

#include "Vulkan/Instance.hpp"
#include "Debug/Debug.hpp"

namespace HelloVikingRoom::Vulkan {
    /* Wraps the Debugger class of Vulkan. */
    class Debugger {
    private:
        /* Vulkan's debug messenger that we're wrapping. */
        VkDebugUtilsMessengerEXT vk_debugger;
        /* Pointer to the destroy function used by Vulkan, which we'll use in the destructor. */
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

    public:
        /* Instance where the debugger is bound to. */
        const Instance& instance;

        /* Constructor for the Debugger class, which takes an Instance class to bind the debugger to. */
        Debugger(const Instance& instance);
        /* Copy constructor for the Debugger class, which is deleted alas. */
        Debugger(const Debugger& other) = delete;
        /* Move constructor for the Debugger class. */
        Debugger(Debugger&& other);
        /* Destructor for the Debugger class. */
        ~Debugger();

        /* Static callback function for Vulkan to call whenever it wants to log something. */
        static VKAPI_ATTR VkBool32 VKAPI_CALL vk_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* user_data
        );

        /* Populates a given create_info struct for the VkDebugUtilsMessengerEXT class. */
        static void populate_messenger_info(VkDebugUtilsMessengerCreateInfoEXT* debug_info);

        /* Explicity retrieves the internal VkDebugUtilsMessengerEXT class. */
        inline VkDebugUtilsMessengerEXT debugger() const { return this->vk_debugger; }
        /* Enable implicit conversion to the VkDebugUtilsMessengerEXT class. */
        inline operator VkDebugUtilsMessengerEXT() const { return this->vk_debugger; }
    };
}

#endif
