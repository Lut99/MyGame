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
    device(device)
{
    DENTER("Vulkan::ShaderModule::ShaderModule");
    DLOG(info, "Loading Vulkan shader module '" + path + "'...");

    // First, retrieve the current directory's filepath
    

    // First, open a file handle (from the end of the file, ate, instead of the beginning) to try and load the given file as raw binary data
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        char buffer[BUFSIZ];
        strerror_s(buffer, BUFSIZ, errno);
        DLOG(fatal, "Failed to open shader file '" + path + "': " + buffer);
    }

    // Since we're at the end of the file, read the position to know the size of our file buffer
    size_t file_size = file.tellg();
    size_t n_ints = file_size / 4 + (file_size % 4 == 0 ? 0 : 1);
    uint32_t* raw_buffer = new uint32_t[n_ints];

    // Go the beginning of the file and fill the buffer
    file.seekg(0);
    file.read((char*) raw_buffer, file_size);
    file.close();

    // Use the allocated buffer to create the VkShaderModule object
    VkShaderModuleCreateInfo shader_module_info{};
    shader_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    // Add the pointer to the spv code
    shader_module_info.codeSize = file_size;
    shader_module_info.pCode = raw_buffer;

    // Create the module
    if (vkCreateShaderModule(this->device, &shader_module_info, nullptr, &this->vk_shader_module) != VK_SUCCESS) {
        delete[] raw_buffer;
        DLOG(fatal, "Could not create shader module.");
    }

    // We're done, so deallocate the raw buffer
    delete[] raw_buffer;

    DLEAVE;
}

/* Move constructor for the ShaderModule class. */
ShaderModule::ShaderModule(ShaderModule&& other) :
    vk_shader_module(other.vk_shader_module),
    device(device)
{
    other.vk_shader_module = nullptr;
}

/* Destructor for the ShaderModule class. */
ShaderModule::~ShaderModule() {
    DENTER("Vulkan::ShaderModule::~ShaderModule");
    DLOG(info, "Cleaning Vulkan shader module...");

    if (this->vk_shader_module != nullptr) {
        vkDestroyShaderModule(this->device, this->vk_shader_module, nullptr);
    }

    DLEAVE;
}
