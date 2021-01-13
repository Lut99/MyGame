/* SHADER MODULE.cpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 12:04:23
 * Last edited:
 *   13/01/2021, 12:04:23
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Class that compiles and wraps a target shader, and then manages the
 *   wrapped VkShaderModule object.
**/

#include <fstream>
#include <cstring>
#include <cerrno>

#include "Tools/Array.hpp"
#include "Debug/Debug.hpp"
#include "ShaderModule.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** SHADERMODULE CLASS *****/
/* Constructor for the ShaderModule class, which takes the device to compile the shader for and the path of the .spv file to load. */
ShaderModule::ShaderModule(const Device& device, const std::string& path) :
    vk_shader_module(nullptr),
    device(device),
    path(path)
{
    DENTER("Vulkan::ShaderModule::ShaderModule");
    DLOG(auxillary, "Loading Vulkan shader module '" + path + "'...");

    // First, open a file handle (from the end of the file, ate, instead of the beginning) to try and load the given file as raw binary data
    std::ifstream file(this->path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        char buffer[BUFSIZ];
        strerror_s(buffer, BUFSIZ, errno);
        DLOG(fatal, "Failed to open shader file '" + this->path + "': " + buffer);
    }

    // Since we're at the end of the file, read the position to know the size of our file buffer
    size_t file_size = file.tellg();
    this->shader_data.resize(file_size);

    // Go the beginning of the file and fill the buffer
    file.seekg(0);
    file.read(this->shader_data.data(), file_size);
    file.close();

    // Use the allocated buffer to create the VkShaderModule object
    VkShaderModuleCreateInfo shader_module_info{};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    // Add the pointer to the spv code
    shader_module_info.codeSize = this->shader_data.size();
    shader_module_info.pCode = reinterpret_cast<uint32_t*>(this->shader_data.data());

    // Create the module & we're done!
    if (vkCreateShaderModule(this->device, &shader_module_info, nullptr, &this->vk_shader_module) != VK_SUCCESS) {
        DLOG(fatal, "Could not create shader module.");
    }

    DLEAVE;
}

/* Copy constructor for the ShaderModule class. */
ShaderModule::ShaderModule(const ShaderModule& other) :
    shader_data(other.shader_data),
    vk_shader_module(nullptr),
    device(other.device),
    path(other.path)
{
    DENTER("Vulkan::ShaderModule::ShaderModule(copy)");

    // Use the allocated buffer to create the VkShaderModule object
    VkShaderModuleCreateInfo shader_module_info{};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    // Add the pointer to the spv code
    shader_module_info.codeSize = this->shader_data.size();
    shader_module_info.pCode = reinterpret_cast<uint32_t*>(this->shader_data.data());

    // Create the module & we're done!
    if (vkCreateShaderModule(this->device, &shader_module_info, nullptr, &this->vk_shader_module) != VK_SUCCESS) {
        DLOG(fatal, "Could not create shader module.");
    }

    DLEAVE;
}

/* Move constructor for the ShaderModule class. */
ShaderModule::ShaderModule(ShaderModule&& other) :
    shader_data(other.shader_data),
    vk_shader_module(other.vk_shader_module),
    device(other.device),
    path(other.path)
{
    other.vk_shader_module = nullptr;
}

/* Destructor for the ShaderModule class. */
ShaderModule::~ShaderModule() {
    DENTER("Vulkan::ShaderModule::~ShaderModule");
    DLOG(auxillary, "Cleaning Vulkan shader module...");

    if (this->vk_shader_module != nullptr) {
        vkDestroyShaderModule(this->device, this->vk_shader_module, nullptr);
    }

    DLEAVE;
}
