/* DESCRIPTOR SET LAYOUT.cpp
 *   by Lut99
 *
 * Created:
 *   15/01/2021, 22:14:33
 * Last edited:
 *   15/01/2021, 22:14:33
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorSetLayout class, which can be used to bind
 *   descriptors (like uniform buffers) to certain shaders, where the data in
 *   them can be used.
**/

#include "Debug/Debug.hpp"
#include "DescriptorSetLayout.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** UNIFORMBUFFER CLASS *****/
/* Constructor for the DescriptorSetLayout class, which takes a device to bind the buffer to and the shader stage where the uniform buffer will eventually be bound to. */
DescriptorSetLayout::DescriptorSetLayout(const Device& device, VkShaderStageFlags shader_stage) :
    device(device)
{
    DENTER("Vulkan::DescriptorSetLayout::DescriptorSetLayout");
    DLOG(auxillary, "Defining Vulkan descriptor set layout...");

    // Define how to bind the layout descriptor (i.e., where)
    VkDescriptorSetLayoutBinding descriptor_set_binding{};
    // Set the index of the binding, must be equal to the place in the shader
    descriptor_set_binding.binding = 0;
    // Set the type of the descriptor
    descriptor_set_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    // Set the number of descriptors to use
    descriptor_set_binding.descriptorCount = 1;
    // Set the stage flags
    descriptor_set_binding.stageFlags = shader_stage;
    // For now, we won't use the next field, as this is for multi-sampling and we don't use that yet
    descriptor_set_binding.pImmutableSamplers = nullptr;

    // Next, we'll create the DescriptorSetLayout itself via a create info (who could've guessed)
    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
    descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    // Set the bindings to use
    descriptor_set_layout_info.bindingCount = 1;
    descriptor_set_layout_info.pBindings = &descriptor_set_binding;
    if (vkCreateDescriptorSetLayout(this->device, &descriptor_set_layout_info, nullptr, &this->vk_descriptor_set_layout) != VK_SUCCESS) {
        DLOG(fatal, "Could not create descriptor set layout.");
    }

    DLEAVE;
}

/* Move constructor for the DescriptorSetLayout class. */
DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) :
    vk_descriptor_set_layout(other.vk_descriptor_set_layout),
    device(other.device)
{
    other.vk_descriptor_set_layout = nullptr;
}

/* Destructor for the DescriptorSetLayout class. */
DescriptorSetLayout::~DescriptorSetLayout() {
    DENTER("Vulkan::DescriptorSetLayout::~DescriptorSetLayout");
    DLOG(info, "Cleaning Vulkan descriptor set layout...");

    if (this->vk_descriptor_set_layout != nullptr) {
        vkDestroyDescriptorSetLayout(this->device, this->vk_descriptor_set_layout, nullptr);
    }

    DLEAVE;
}
