/* TEXTURE SAMPLER.hpp
 *   by Lut99
 *
 * Created:
 *   16/01/2021, 17:16:34
 * Last edited:
 *   16/01/2021, 17:16:34
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the TextureSampler class, which is the C++-interface to the
 *   Vulkan samplers for textures. Used to be less errory and nicer to
 *   textures than just the raw image.
**/

#ifndef VULKAN_TEXTURE_SAMPLER_HPP
#define VULKAN_TEXTURE_SAMPLER_HPP

#include <vulkan/vulkan.h>

#include "Vulkan/Device.hpp"
#include "Vulkan/Image.hpp"

namespace HelloVikingRoom::Vulkan {
    /* The TextureSampler class, which samples textures so that they look nicer and fit better to the rendering process. */
    class TextureSampler {
    private:
        /* The VkSampler that this class wraps. */
        VkSampler vk_sampler;
    
    public:
        /* Constant reference to the device where the sampler lives. */
        const Device& device;

        /* Constructor for the TextureSampler class, which takes a device where it shall live. */
        TextureSampler(const Device& device);
        /* Copy constructor for the TextureSampler, which is deleted. */
        TextureSampler(const TextureSampler& other) = delete;
        /* Move constructor for the TextureSampler. */
        TextureSampler(TextureSampler&& other);
        /* Destructor for the TextureSampler class. */
        ~TextureSampler();

        /* Expliticly returns the internal VkSampler object. */
        inline const VkSampler& sampler() const { return this->vk_sampler; }
        /* Implicitly casts this class to a VkSampler by returning the internal object. */
        inline operator VkSampler() const { return this->vk_sampler; }

    };
}

#endif
