/* DESCRIPTOR SET LAYOUT.hpp
 *   by Lut99
 *
 * Created:
 *   15/01/2021, 22:14:37
 * Last edited:
 *   15/01/2021, 22:14:37
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the DescriptorSetLayout class, which can be used to bind
 *   descriptors (like uniform buffers) to certain shaders, where the data in
 *   them can be used.
**/

#ifndef VULKAN_UNIFORM_BUFFER_HPP
#define VULKAN_UNIFORM_BUFFER_HPP

#include <vulkan/vulkan.h>

#include "Device.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The DescriptorSetLayout class is used to describe how to bind a descriptor (like a uniform buffer) to a shader. */
    class DescriptorSetLayout {
    private:
        /* The descriptor used to tell Vulkan where and how to bind this buffer to a shader. */
        VkDescriptorSetLayout vk_descriptor_set_layout;

    public:
        /* The device to which this descriptor set layout is bound. */
        const Device& device;

        /* Constructor for the DescriptorSetLayout class, which takes a device to bind the buffer to and the shader stage where the uniform buffer will eventually be bound to. */
        DescriptorSetLayout(const Device& device, VkShaderStageFlags shader_stage);
        /* Copy constructor for the DescriptorSetLayout class, which is deleted. */
        DescriptorSetLayout(const DescriptorSetLayout& other) = delete;
        /* Move constructor for the DescriptorSetLayout class. */
        DescriptorSetLayout(DescriptorSetLayout&& other);
        /* Destructor for the DescriptorSetLayout class. */
        virtual ~DescriptorSetLayout();

        /* Expliticly returns the internal VkDescriptorSetLayout object. */
        inline const VkDescriptorSetLayout& descriptor_set_layout() const { return this->vk_descriptor_set_layout; }
        /* Implicitly casts this class to a VkDescriptorSetLayout by returning the internal object. */
        inline operator VkDescriptorSetLayout() const { return this->vk_descriptor_set_layout; }

    };
}

#endif
