/* SHADER MODULE.hpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 12:04:27
 * Last edited:
 *   13/01/2021, 12:04:27
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that compiles and wraps a target shader, and then manages the
 *   wrapped VkShaderModule object.
**/

#ifndef VULKAN_SHADER_MODULE_HPP
#define VULKAN_SHADER_MODULE_HPP

#include <string>
#include <vector>

#include "Device.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The ShaderModule class, which loads the shader at the given path and manages the internal object. */
    class ShaderModule {
    private:
        /* The raw shader data parsed from a file. */
        std::vector<char> shader_data;
        /* The internal VkShaderModule object that this class wraps. */
        VkShaderModule vk_shader_module;

    public:
        /* Constant reference to the Device that this module is compiled for. */
        const Device& device;
        /* Path that this ShaderModule is loaded from. */
        const std::string path;

        /* Constructor for the ShaderModule class, which takes the device to compile the shader for and the path of the .spv file to load. */
        ShaderModule(const Device& device, const std::string& path);
        /* Copy constructor for the ShaderModule class. */
        ShaderModule(const ShaderModule& other);
        /* Move constructor for the ShaderModule class. */
        ShaderModule(ShaderModule&& other);
        /* Destructor for the ShaderModule class. */
        ~ShaderModule();

        /* Expliticly retrieves the internal VkShaderModule object. */
        inline VkShaderModule shader_module() const { return this->vk_shader_module; }
        /* Implicitly casts this class to a VkShaderModule object, returning the internal VkShaderModule. */
        inline operator VkShaderModule() const { return this->vk_shader_module; }
    };
}

#endif
