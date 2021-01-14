/* DEVICE.hpp
 *   by Lut99
 *
 * Created:
 *   24/12/2020, 13:37:09
 * Last edited:
 *   24/12/2020, 13:37:09
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The Device class wraps a Vulkan VkDevice struct. Upon allocation,
 *   automatically selects a suitable GPU to use.
**/

#ifndef VULKAN_DEVICE_HPP
#define VULKAN_DEVICE_HPP

#include "Vulkan/Instance.hpp"

namespace HelloVikingRoom::Vulkan {
    /* Class that stores the queue family indices for a device. */
    class DeviceQueueInfo {
    private:
        /* The index of the graphics family on a device. */
        uint32_t graphics_index;
        /* Boolean that keeps track of the whether or not the graphics queue is supported. */
        bool supports_graphics;

        /* The index of the presentation queue family on a device. */
        uint32_t presenting_index;
        /* Boolean that keeps track of the whether or not the presentation queue is supported. */
        bool supports_presenting;
    
    public:
        /* Default constructor for the DeviceQueueInfo class, which takes a VkPhysicalDevice to derive which queues are supported or not and a surface to check if the device can present to it. */
        DeviceQueueInfo(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);

        /* Returns the index of the found graphics queue. Undefined behaviour for when the graphics queue is not supported. */
        inline uint32_t graphics() const { return this->graphics_index; }
        /* Returns the index of the found presentation queue. Undefined behaviour for when the presentation queue is not supported. */
        inline uint32_t presentation() const { return this->presenting_index; }
        /* Returns a list of all indices stored in this class. */
        inline Array<uint32_t> indices() const { return Array<uint32_t>({ this->graphics_index, this->presenting_index }); }

        /* Returns whether or not the graphics queue is supported or not. */
        inline bool graphics_supported() const { return this->supports_graphics; }
        /* Returns whether or not the presentation queue is supported or not. */
        inline bool presentation_supported() const { return this->supports_presenting; }
        /* Returns whether or not all relevant (graphics & presentation) queues are supported. */
        inline bool supported() const { return this->supports_graphics && this->supports_presenting; }

    };

    /* Class that stores information on the support capabalities of a device's swapchain. */
    class DeviceSwapchainInfo {
    private:
        /* Lists the capabilities of our swap chain in Vulkan-terms. */
        VkSurfaceCapabilitiesKHR swapchain_capabalities;
        /* Lists the formats (i.e., pixel colour space etc) that the swap chain supports. */
        Array<VkSurfaceFormatKHR> swapchain_formats;
        /* Lists the presentation modes that the swap chain supports. */
        Array<VkPresentModeKHR> swapchain_present_modes;

    public:
        /* Constructor for the DeviceSwapchainInfo, which takes a physical device and a surface to check how the device's swapchain supports the surface. */
        DeviceSwapchainInfo(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface);

        /* Returns the capabilities of the device's swapchain as a VkSurfaceCapabilitiesKHR struct. */
        inline const VkSurfaceCapabilitiesKHR& capabalities() const { return this->swapchain_capabalities; }
        /* Returns the formats supported by the device's swapchain as an array of VkSurfaceFormatKHR structs. */
        inline const Array<VkSurfaceFormatKHR>& formats() const { return this->swapchain_formats; }
        /* Returns the present modes supported by the device's swapchain as an array of VkSurfacVkPresentModeKHReFormatKHR structs. */
        inline const Array<VkPresentModeKHR>& present_modes() const { return this->swapchain_present_modes; }
    };



    /* The Device class wraps a VkDevice, automatically selecting a suitable physical device to use. */
    class Device {
    private:
        /* The VkPhysicalDevice we selected. */
        VkPhysicalDevice vk_physical_device;
        /* The VkDevice that wraps the physical device. */
        VkDevice vk_device;
        /* The struct containing the queue information for this device. */
        DeviceQueueInfo* queue_info;
        /* The struct containing the swapchain information for this device. */
        DeviceSwapchainInfo* swapchain_info;

        /* Handle for the graphics queue of the device. */
        VkQueue vk_graphics_queue;
        /* Handle for the graphics queue of the device. */
        VkQueue vk_presentation_queue;

        /* String that stores the name of the selected GPU. */
        std::string gpu_name;

    public:
        /* The Vulkan instance we wrap. */
        const Instance& instance;

        
        /* Constructor for the Device class, which takes a Vulkan Instance to bind the chosen GPU to, a VkSurfaceKHR struct to check if GPUs can present to our surface and a list of extensions the device should support. */
        Device(const Instance& instance, const VkSurfaceKHR& surface, const Array<const char*>& device_extensions);
        /* Copy constructor for the Device class, which is deleted, since we work with handles. */
        Device(const Device& other) = delete;
        /* Move constructor for the Device class. */
        Device(Device&& other);
        /* Destructor for the Device class. */
        ~Device();
        
        /* Static function that determines whether or not a given GPU supports the given list of extensions. */
        static bool gpu_supports_extensions(const VkPhysicalDevice& physical_device, const Array<const char*>& device_extensions);
        /* Static function that determines when a GPU is suitable. */
        static bool is_suitable_gpu(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, const Array<const char*>& device_extensions);
        /* Static function that helps selecting the correct GPU. */
        static VkPhysicalDevice pick_gpu(const Instance& instance, const VkSurfaceKHR& surface, const Array<const char*>& device_extensions);

        /* Returns the name of the selected GPU. */
        inline std::string name() const { return this->gpu_name; }
        /* Returns the queue indices of this Device. */
        inline Array<uint32_t> indices() const { return this->queue_info->indices(); }

        /* Returns the handle for the graphics queue of the GPU. */
        inline const VkQueue& graphics_queue() const { return this->vk_graphics_queue; }
        /* Returns the handle for the presentation queue of the GPU. */
        inline const VkQueue& presentation_queue() const { return this->vk_presentation_queue; }

        /* Returns a constant reference to the queue information of this device. */
        inline const DeviceQueueInfo& get_queue_info() const { return *this->queue_info; }
        /* Returns a constant reference to the swapchain information of this device. */
        inline const DeviceSwapchainInfo& get_swapchain_info() const { return *this->swapchain_info; }

        /* Explicity retrieves the internal VkPhysicalDevice instance. */
        inline const VkPhysicalDevice& physical_device() const { return this->vk_physical_device; }
        /* Explicity retrieves the internal VkDevice instance. */
        inline const VkDevice& device() const { return this->vk_device; }
        /* Enable implicit conversion to the internal VkPhysicalDevice instance. */
        inline operator VkPhysicalDevice() const { return this->vk_physical_device; }
        /* Enable implicit conversion to the internal VkDevice instance. */
        inline operator VkDevice() const { return this->vk_device; }

    };
}

#endif
