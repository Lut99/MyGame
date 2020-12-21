/* INSTANCE.hpp
 *   by Lut99
 *
 * Created:
 *   21/12/2020, 13:36:01
 * Last edited:
 *   21/12/2020, 13:36:01
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Wrapper for a Vulkan instance, that automatically handles allocation
 *   and deallocation.
**/

#ifndef VULKAN_INSTANCE_HPP
#define VULKAN_INSTANCE_HPP

#include <vulkan/vulkan.h>

#include "Tools/Array.hpp"

namespace HelloVikingRoom::Vulkan {
    /* Wraps around the default Vulkan instance. */
    class Instance {
    private:
        /* The VkInstance that this class wraps. */
        VkInstance vk_instance;

    public:
        /* Constructor for the Instance class, that takes a list of the extensions that we'll require from our Vulkan installation. Does not enable any validation layers or debuggers. */
        Instance(const Tools::Array<const char*>& required_extensions);
        /* Constructor for the Instance class, that takes a list of the extensions that we'll require from our Vulkan installation and a list of validation layers we'll want to enable. */
        Instance(const Tools::Array<const char*>& required_extensions, const Tools::Array<const char*>& required_layers);
        /* Copy constructor for the Instance class, which is deleted because otherwise it wouldn't make much sense. */
        Instance(const Instance& other) = delete;
        /* Move constructor for the Instance class. */
        Instance(Instance&& other);
        /* Destructor for the Instance class. */
        ~Instance();

        /* Explicity retrieves the VkInstance class. */
        inline VkInstance instance() const { return this->vk_instance; }
        /* Enable implicit conversion to the VkInstance class. */
        inline operator VkInstance() const { return this->vk_instance; }
    };
}

#endif
