/* DEVICE.cpp
 *   by Lut99
 *
 * Created:
 *   24/12/2020, 13:41:24
 * Last edited:
 *   24/12/2020, 13:41:24
 * Auto updated?
 *   Yes
 *
 * Description:
 *   <Todo>
**/

#include "Vulkan/Device.hpp"
#include "Tools/Array.hpp"
#include "Debug/Debug.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** DEVICEQUEUEINFO CLASS *****/
/* Default constructor for the DeviceQueueInfo class, which takes a VkPhysicalDevice to derive which queues are supported or not. */
DeviceQueueInfo::DeviceQueueInfo(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface) :
    supports_graphics(false),
    supports_presenting(false)
{
    DENTER("DeviceQueueInfo::DeviceQueueInfo");

    // First, get a list of all the queues supported by the GPU
    uint32_t n_supported_queues;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &n_supported_queues, nullptr);
    Array<VkQueueFamilyProperties> supported_queues(n_supported_queues);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &n_supported_queues, supported_queues.wdata(n_supported_queues));

    // Loop through all the queues the device supports, stopping early when it supports everything we want it to support
    for (size_t i = 0; i < supported_queues.size() && !this->supported(); i++) {
        // If the current queue is a graphics queue, we found it
        if (supported_queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            this->supports_graphics = true;
            this->graphics_index = (uint32_t) i;
        }

        // Check if this queue supports presenting to the given surface
        VkBool32 supports_presenting = VK_FALSE;
        if (vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, (uint32_t) i, surface, &supports_presenting) != VK_SUCCESS) {
            DLOG(warning, "Could not get surface presenting support for queue " + std::to_string(i) + " of GPU");
            this->supports_presenting = false;
        } else if (supports_presenting) {
            // It supports it
            this->supports_presenting = true;
            this->presenting_index = (uint32_t) i;
        }
    }

    // Done
    DLEAVE;
}





/***** DEVICESWAPCHAININFO CLASS *****/
/* Constructor for the DeviceSwapchainInfo, which takes a physical device and a surface to check how the device's swapchain supports the surface. */
DeviceSwapchainInfo::DeviceSwapchainInfo(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface) {
    DENTER("DeviceSwapchainInfo::DeviceSwapchainInfo");

    // Get the capabilities of the GPU's swapchain first
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &this->swapchain_capabalities) != VK_SUCCESS) {
        DLOG(warning, "Could not get swapchain capabilities of GPU");
        this->swapchain_capabalities = {};
    }

    // Next, get a list of all surface formats supported by the swapchain
    uint32_t n_supported_formats = 0;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &n_supported_formats, nullptr) != VK_SUCCESS) {
        DLOG(warning, "Could not get number of supported formats supported by the GPU's swapchain");
    } else {
        // Resize the internal array first
        this->swapchain_formats.reserve(n_supported_formats);
        if (vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &n_supported_formats, this->swapchain_formats.wdata(n_supported_formats)) != VK_SUCCESS) {
            DLOG(warning, "Could not get supported formats supported by the GPU's swapchain");
            this->swapchain_formats.clear();
        }
    }

    // Finally, get a list of all present modes supported by the swapchain
    uint32_t n_present_modes = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &n_present_modes, nullptr) != VK_SUCCESS) {
        DLOG(warning, "Could not get number of present modes supported by the GPU's swapchain");
    } else {
        // Resize the internal array first
        this->swapchain_present_modes.reserve(n_present_modes);
        if (vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &n_present_modes, this->swapchain_present_modes.wdata(n_present_modes)) != VK_SUCCESS) {
            DLOG(warning, "Could not get present modes supported by the GPU's swapchain");
            this->swapchain_present_modes.clear();
        }
    }

    // Done
    DLEAVE;
}





/***** DEVICE CLASS *****/
/* Constructor for the Device class, which takes a Vulkan Instance to bind the chosen GPU to. */
Device::Device(const Instance& instance, const VkSurfaceKHR& surface, const Array<const char*>& device_extensions) :
    instance(instance)
{
    DENTER("Device::Device");
    DLOG(info, "Creating Vulkan device instance...");

    // Pick the correct GPU first
    this->vk_physical_device = this->pick_gpu(this->instance, surface, device_extensions);

    // Get some GPU properties, like its name
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(this->vk_physical_device, &device_properties);
    this->gpu_name = device_properties.deviceName;
    this->queue_info = new DeviceQueueInfo(this->vk_physical_device, surface);
    this->swapchain_info = new DeviceSwapchainInfo(this->vk_physical_device, surface);
    DLOG(auxillary, "Selected GPU with name '" + this->gpu_name + "'");



    // It's time to creating the logical device itself
    DLOG(info, "Creating logical device...");

    // First, collect the queues that we want to use for this logical devices (but only keep unique indices)
    Array<uint32_t> queues_indices = this->queue_info->indices();
    if (queues_indices.at(0) == queues_indices.at(1)) { queues_indices.pop_back(); }
    Array<VkDeviceQueueCreateInfo> queue_infos(queues_indices.size());
    for (size_t i = 0; i < queue_infos.capacity(); i++) {
        // Populate the create struct
        queue_infos.push_back({});
        queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        // Tell the struct we want one queue linked to our current queue family
        queue_infos[i].queueFamilyIndex = queues_indices[i];
        queue_infos[i].queueCount = 1;
        // We given each queue in this create info the same priority (of 1)
        float priority = 1.0f;
        queue_infos[i].pQueuePriorities = &priority;
    }

    // Next, "create" the list features we want for the device (none for now)
    VkPhysicalDeviceFeatures device_features{};

    // Use the queue indices and the features to populate the create info for the device itself
    VkDeviceCreateInfo device_info{};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    // Pass the queues to the struct
    device_info.queueCreateInfoCount = static_cast<uint32_t>(queue_infos.size());
    device_info.pQueueCreateInfos = queue_infos.rdata();
    // Pass the features we enable
    device_info.pEnabledFeatures = &device_features;
    // Tell the struct the extensions we want to enable
    device_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    device_info.ppEnabledExtensionNames = device_extensions.rdata();

    // Create the device handler
    if (vkCreateDevice(this->vk_physical_device, &device_info, nullptr, &this->vk_device) != VK_SUCCESS) {
        DLOG(fatal, "Could not create logical device");
    }

    // Get the actual queues from the device and put them in the queue handles
    vkGetDeviceQueue(this->vk_device, queues_indices[0], 0, &this->vk_graphics_queue);
    vkGetDeviceQueue(this->vk_device, queues_indices[queues_indices.size() > 1 ? 1 : 0], 0, &this->vk_presentation_queue);

    // We're done!
    DLEAVE;
}

/* Move constructor for the Device class. */
Device::Device(Device&& other) :
    instance(other.instance),
    vk_physical_device(other.vk_physical_device),
    vk_device(other.vk_device),
    queue_info(other.queue_info),
    swapchain_info(other.swapchain_info),
    vk_graphics_queue(other.vk_graphics_queue),
    vk_presentation_queue(other.vk_presentation_queue),
    gpu_name(other.gpu_name)
{
    // Set the relevant vulkan structs to nullptr's in the other instance
    other.vk_device = nullptr;

    // Also relevant non-vulkan structs
    other.queue_info = nullptr;
    other.swapchain_info = nullptr;
}

/* Destructor for the Device class. */
Device::~Device() {
    // Only destroy the device if not nullptr
    if (this->vk_device != nullptr) {
        vkDestroyDevice(this->vk_device, nullptr);
    }
    // Only destroy the devices infos if not nullptr
    if (this->queue_info != nullptr) { delete this->queue_info; }
    if (this->swapchain_info != nullptr) { delete this->swapchain_info; }
}



/* Static function that determines whether or not a given GPU supports the given list of extensions. */
bool Device::gpu_supports_extensions(const VkPhysicalDevice& physical_device, const Array<const char*>& device_extensions) {
    DENTER("Device::gpu_supports_extensions");

    // Get a list of all extensions supported by the device
    uint32_t n_supported_extensions = 0;
    if (vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &n_supported_extensions, nullptr) != VK_SUCCESS) {
        DLOG(warning, "Could not count of supported extensions of GPU");
        DRETURN false;
    }
    Array<VkExtensionProperties> supported_extensions(n_supported_extensions);
    if (vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &n_supported_extensions, supported_extensions.wdata(n_supported_extensions)) != VK_SUCCESS) {
        DLOG(warning, "Could not supported extensions of GPU");
        DRETURN false;
    }

    // Use a loop to check if all our extensions are supported
    for (size_t i = 0; i < device_extensions.size(); i++) {
        bool found = false;
        for (size_t i = 0; i < supported_extensions.size(); i++) {
            if (strcmp(device_extensions[i], supported_extensions[i].extensionName) != 0) {
                found = true;
                break;
            }
        }
        if (!found) {
            // Missing extension
            DRETURN false;
        }
    }

    // We made the loop, meaning we found all extensions; we're done here
    DRETURN true;
}

/* Static function that determines when a GPU is suitable. */
bool Device::is_suitable_gpu(const VkPhysicalDevice& physical_device, const VkSurfaceKHR& surface, const Array<const char*>& device_extensions) {
    DENTER("Device::is_suitable_gpu");

    // First, get a list of supported extensions on this device
    DeviceQueueInfo queue_info(physical_device, surface);

    // Next, check if the device supports the extensions we want it to support
    bool supports_extensions = Device::gpu_supports_extensions(physical_device, device_extensions);

    // If it supports our desired extensions (i.e., the swapchain), check if the swapchain can write to the formats required by our surface
    bool supports_swapchain = false;
    if (supports_extensions) {
        // Get the information for this swapchain
        DeviceSwapchainInfo swapchain_info(physical_device, surface);
        supports_swapchain = !swapchain_info.formats().empty() && !swapchain_info.present_modes().empty();
    }

    // Using the information collected above, we can determine if the device is good enough
    DRETURN queue_info.supported() && supports_extensions && supports_swapchain;
}

/* Static function that helps selecting the correct GPU. */
VkPhysicalDevice Device::pick_gpu(const Instance& instance, const VkSurfaceKHR& surface, const Array<const char*>& device_extensions) {
    DENTER("Device::pick_gpu");
    DLOG(auxillary, "Selecting GPU to use...");

    // Get how many Vulkan-capable devices there are
    uint32_t n_available_devices = 0;
    if (vkEnumeratePhysicalDevices(instance, &n_available_devices, nullptr) != VK_SUCCESS) {
        DLOG(fatal, "Could not get the number of available GPUs");
    }
    if (n_available_devices == 0) {
        DLOG(fatal, "No Vulkan-compatible GPUs found");
    }

    // If there are GPUs, get a list of them
    Array<VkPhysicalDevice> available_devices(n_available_devices);
    if (vkEnumeratePhysicalDevices(instance, &n_available_devices, available_devices.wdata(n_available_devices)) != VK_SUCCESS) {
        DLOG(fatal, "Could not get a list of available GPUs");
    }

    // Find the first suitable device
    for (size_t i = 0; i < available_devices.size(); i++) {
        // Just take the first GPU we see
        if (Device::is_suitable_gpu(available_devices[i], surface, device_extensions)) {
            DRETURN available_devices[i];
        }
    }

    // Otherwise, we weren't able to find anything
    DLOG(fatal, "Could not find a suitable GPU");
    DRETURN nullptr;
}
