/* MAIN.cpp
 *   by Lut99
 *
 * Created:
 *   16/12/2020, 13:34:59
 * Last edited:
 *   16/12/2020, 23:55:30
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File used to follow the hello_triangle tutorial of Vulkan
**/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <unordered_map>


/***** CONSTANTS *****/
/* Prefix for all Vulkan messages. */
#define VULKAN "[\033[1mVULKAN\033[0m] "
/* Prefix for each logging message. */
#define INFO "[\033[36;1m INFO \033[0m] "
/* Prefix for each warning. */
#define WARNING "[\033[33;1m WARN \033[0m] "
/* Prefix for each error. */
#define ERROR "[\033[31;1m ERRR \033[0m] "
/* Prefix for each other message. */
#define EMPTY "         "





/***** VULKAN PROXY FUNCTIONS *****/
#ifdef DEBUG
/* Proxy for atomatically loading the vkCreateDebugUtilsMessengerEXT function. */
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        // Call the function
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        // Didn't find it!
        throw std::runtime_error(ERROR "Coult not load vkCreateDebugUtilsMessengerEXT");
    }
}
/* Proxy for atomatically loading the vkDestroyDebugUtilsMessengerEXT function. */
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        // Call the function
        func(instance, debugMessenger, pAllocator);
    } else {
        // Didn't find it!
        throw std::runtime_error(ERROR "Coult not load vkDestroyDebugUtilsMessengerEXT");
    }
}
#endif





/***** DEBUGGING CALLBACKS *****/
#ifdef DEBUG
/* Callback for the standard debug case. */
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void*) {
    // Determine the logging prefix based on the 
    const char* log_prefix;
    switch(message_severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            log_prefix = INFO;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            log_prefix = WARNING;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            log_prefix = ERROR;
            break;
        default:
            log_prefix = "[????] ";
            break;
    }

    // Determine what type of message this is
    const char* log_type;
    switch(message_type) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            log_type = "GENERAL";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            log_type = "VALIDATION";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            log_type = "PERFORMANCE";
            break;
        default:
            log_type = "????";
            break;
    }

    // Write the message, with the correct prefixes
    std::cerr << log_prefix << VULKAN << pCallbackData->pMessage << " (" << log_type << ")" << std::endl;
    return VK_FALSE;
}
#endif




/***** HELPER STRUCTS *****/
/* Struct that is used to examine the queues present on a given device. */
struct DeviceQueueSupport {
    /* Marks whether or not the device supports graphics capabilities. */
    bool supports_graphics;
    /* Marks whether or not the device supports presenting to our given surface. */
    bool supports_presenting;

    /* If it has a value, then the device supports graphics queues. */
    uint32_t graphics;
    /* If it is supported, then this value stores the queue family index. */
    uint32_t presenting;

    /* Returns true if all interesting queues have been found, or false otherwise. */
    inline bool is_supported() const { return supports_graphics && supports_presenting; }
};

/* Struct that is used to examine if the swap chain supports what we want. */
struct SwapChainSupport {
    /* Lists the capabilities of our swap chain in Vulkan-terms. */
    VkSurfaceCapabilitiesKHR capabalities;
    /* Lists the formats (i.e., pixel colour space etc) that the swap chain supports. */
    std::vector<VkSurfaceFormatKHR> formats;
    /* Lists the presentation modes that the swap chain supports. */
    std::vector<VkPresentModeKHR> presentModes;
};





/***** MAIN CLASS *****/
/* Class that wraps our application. */
class HelloTriangleApplication {
private:
    /* Extensions that we require to be enabled on the GPU. */
    std::vector<const char*> device_extensions;
    /* Validation layers enabled when the DEBUG-flag is specified. */
    std::vector<const char*> validation_layers;

    /* The Window */
    GLFWwindow* window;
    /* The Vulkan instance */
    VkInstance instance;
    /* The surface area, which we can use to draw on. */
    VkSurfaceKHR surface;

    #ifdef DEBUG
    /* The debug messenger of Vulkan. */
    VkDebugUtilsMessengerEXT debug_messenger;
    #endif

    /* The physical device that we'll be using. */
    VkPhysicalDevice gpu;
    
    /* The logical device that we'll be using. */
    VkDevice device;
    /* Handle to the graphics queue that we'll be using. */
    VkQueue graphics_queue;
    /* Handle for the presentations queue that we'll be using. */
    VkQueue present_queue;

    /* The swapchain that we're going to write frames to. */
    VkSwapchainKHR swapchain;
    /* Handle to all the images in the swapchain. */
    std::vector<VkImage> swapchain_frames;
    /* Handle to imageviews to the swapchain images, s.t. we can comfortable work with them. */
    std::vector<VkImageView> swapchain_frameviews;
    /* The format of the swapchain, i.e., the color space and format. */
    VkFormat swapchain_format;
    /* The extent of the swapchain, i.e., the resolution of its frames. */
    VkExtent2D swapchain_extent;



    /* Handlers initialization of a GLFW window. */
    void init_window() {
        #ifdef DEBUG
        std::cout << INFO "Initializing window..." << std::endl;
        #endif

        // First, initialize the library
        glfwInit();

        // Next, tell it to create a non-OpenGL window
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Also let it automatically handle window resizeing
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // Finally, initialize a window in the function
        this->window = glfwCreateWindow(this->wwidth, this->wheight, "Vulkan", nullptr, nullptr);
    }

    

    /* Returns a list of all the extensions we'll require. */
    std::vector<const char*> get_extensions() {
        // Get all the extensions for our window library
        uint32_t n_glfw_extensions = 0;
        const char** glfw_extensions;
        glfw_extensions = glfwGetRequiredInstanceExtensions(&n_glfw_extensions);

        // Put them in the result vector
        std::vector<const char*> result(glfw_extensions, glfw_extensions + n_glfw_extensions);

        #ifdef DEBUG
        // Add the extension for our debugging system
        result.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif

        // Return the list
        return result;
    }

    /* Checks if all required extensions are present. */
    void check_extensions(const std::vector<const char*>& required_extensions) {
        #ifdef DEBUG
        std::cout << INFO "Checking extension support..." << std::endl;
        #endif

        // Start by getting a list of all supported extensions (first just the number, then the list)
        uint32_t n_existing_extensions = 0;
        if (vkEnumerateInstanceExtensionProperties(nullptr, &n_existing_extensions, nullptr) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get supported extensions (count).");
        }
        VkExtensionProperties existing_extensions[n_existing_extensions];
        if (vkEnumerateInstanceExtensionProperties(nullptr, &n_existing_extensions, existing_extensions) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get supported extensions.");
        }

        // Loop to find any missing ones
        bool errored = false;
        for (size_t i = 0; i < required_extensions.size(); i++) {
            // Check if it exists
            bool found = false;
            for (uint32_t j = 0; j < n_existing_extensions; j++) {
                if (strcmp(required_extensions[i], existing_extensions[j].extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                #ifdef DEBUG
                std::cerr << EMPTY WARNING "Extension '" << required_extensions[i] << "' is not supported" << std::endl;
                #endif
                errored = true;
            }
        }
        if (errored) { throw std::runtime_error(ERROR "Missing extensions, cannot continue."); }
    }



    #ifdef DEBUG
    /* Enables Vulkan's validation layers if the system supports it. */
    void trim_validation_layers() {
        std::cout << EMPTY "Checking validation layer support..." << std::endl;

        // First, get the number of layers
        uint32_t n_layers = 0;
        if (vkEnumerateInstanceLayerProperties(&n_layers, nullptr) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get supported validation layers (count).");
        }

        // Extend that to get layer information
        VkLayerProperties existing_layers[n_layers];
        if (vkEnumerateInstanceLayerProperties(&n_layers, existing_layers) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get supported validation layers.");
        }

        // Now, remove the ones that aren't supported
        for (std::vector<const char*>::iterator iter = this->validation_layers.begin(); iter != this->validation_layers.end(); iter++) {
            bool found = false;
            for (uint32_t j = 0; j < n_layers; j++) {
                if (strcmp(*iter, existing_layers[j].layerName) == 0) {
                    // We found a match, so we can stop searching
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cerr << EMPTY WARNING "Validation layer '" << *iter << "' not supported; cannot enable it" << std::endl;
                this->validation_layers.erase(iter);
                return;
            }
        }
    }
    #endif
    

    
    /* Initializes the Vulkan instance, which takes the number of GLFW extensions required and their names */
    VkInstance create_vulkan_instance(const std::vector<const char*>& required_extensions) {
        #ifdef DEBUG
        std::cout << INFO "Creating Vulkan instance..." << std::endl;
        #endif

        // Define some optional application information, useful for optimizing stuff
        VkApplicationInfo appInfo{};
        // For Vulkan structs, always specify the type
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        // Define the application's name
        appInfo.pApplicationName = "Hello triangle";
        // Define the application's version
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        // Note if we're using a specific engine (we're not, though)
        appInfo.pEngineName = "No Engine";
        // Also note the engine's version
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        // Finally, define the version of the API we're using
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Then, make sure to mention which global extensions & validation layers to use
        VkInstanceCreateInfo createInfo{};
        // For Vulkan structs, always specify the type
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        // Link the application info to this application
        createInfo.pApplicationInfo = &appInfo;
        // Next, tell Vulkan how many extensions we'll enable...
        createInfo.enabledExtensionCount = static_cast<uint32_t>(required_extensions.size());
        // ...and which extensions
        createInfo.ppEnabledExtensionNames = required_extensions.data();
        #ifdef DEBUG
        // Make sure that the list of validation layers is supported
        this->trim_validation_layers();

        // Set them in the struct
        createInfo.enabledLayerCount = this->validation_layers.size();
        createInfo.ppEnabledLayerNames = this->validation_layers.data();

        // Additionally, add the debug handler to the createInfo struct s.t. it is debugged properly as well
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        this->populate_vulkan_debug_createInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        #else
        // We don't want any layers or debugging information
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
        #endif

        // Finally, it's time to make the instance!
        VkInstance result;
        if (vkCreateInstance(&createInfo, nullptr, &result) != VK_SUCCESS) {
            // Could not create an instance
            throw std::runtime_error(ERROR "Could not create a Vulkan instance.");
        }
        return result;
    }



    #ifdef DEBUG
    /* Populates a given VkDebugUtilsMessengerCreateInfoEXT struct. */
    void populate_vulkan_debug_createInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        // For Vulkan structs, always specify the type
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        // Define the severities we'll get called for
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        // Define the message types we'll get called for
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        // Mark what our callback is
        createInfo.pfnUserCallback = debugCallback;
        // Optionally, provide a struct to pass to the callback
        createInfo.pUserData = nullptr;
    }

    /* Initializes the debug messenger by registering our callback function. */
    VkDebugUtilsMessengerEXT init_vulkan_debug() {
        std::cout << INFO "Setting up debug messenger..." << std::endl;

        // Create the debug messenger by using a createInfo struct
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populate_vulkan_debug_createInfo(createInfo);

        // Now, use our proxy function to register the callback
        VkDebugUtilsMessengerEXT result;
        if (CreateDebugUtilsMessengerEXT(this->instance, &createInfo, nullptr, &result) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not create the debug messenger.");
        }
        return result;
    }
    #endif



    /* Initializes the surface we'll draw on. */
    VkSurfaceKHR create_surface() {
        // Simply create it, done
        VkSurfaceKHR result;
        if (glfwCreateWindowSurface(this->instance, this->window, nullptr, &result) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not create window surface");
        }
        return result;
    }



    /* Returns the support details of the swap chain of the chosen device. */
    SwapChainSupport get_swapchain_support(VkPhysicalDevice gpu) {
        SwapChainSupport result;

        // First, get the capabilities of the given gpu
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, this->surface, &result.capabalities) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Failed to get surface capabilities of the physical device.");
        }

        // Next, we get the surface formats by the familiar count/properties call
        uint32_t n_formats = 0;
        if (vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, this->surface, &n_formats, nullptr) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Failed to get surface format (count) of the physical device.");
        }
        if (n_formats != 0) {
            result.formats.resize(n_formats);
            if (vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, this->surface, &n_formats, result.formats.data()) != VK_SUCCESS) {
                throw std::runtime_error(ERROR "Failed to get surface format of the physical device.");
            }
        }

        // Finally, to get the presentation modes we do the same joke
        uint32_t n_modes = 0;
        if (vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, this->surface, &n_modes, nullptr) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Failed to get surface present mode (count) of the physical device.");
        }
        if (n_modes != 0) {
            result.presentModes.resize(n_modes);
            if (vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, this->surface, &n_modes, result.presentModes.data()) != VK_SUCCESS) {
                throw std::runtime_error(ERROR "Failed to get surface present mode of the physical device.");
            }
        }

        // We're done here
        return result;
    }
    
    /* Examines the type of Vulkan queues that the given device supports. */
    DeviceQueueSupport get_device_queues(VkPhysicalDevice gpu) {
        DeviceQueueSupport result;

        // First, get the number of queue families that the device supports
        uint32_t n_queues = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &n_queues, nullptr);

        // Next, use that number to get a list of the actually supported queue families
        VkQueueFamilyProperties queues[n_queues];
        vkGetPhysicalDeviceQueueFamilyProperties(gpu, &n_queues, queues);

        // Search through those queues to populate our DeviceQueueSupport struct, with an early stop when it already supports everything we like
        for (uint32_t i = 0; !result.is_supported() && i < n_queues; i++) {
            // Check if it supports graphics capabilities, and store the family index
            if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                result.supports_graphics = true;
                result.graphics = i;
            }

            // Check if it has presenting support to our surface
            VkBool32 presenting_support = VK_FALSE;
            if (vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, this->surface, &presenting_support) != VK_SUCCESS) {
                throw std::runtime_error(ERROR "Failed to get surface support of the physical device.");
            }
            if (presenting_support) {
                result.supports_presenting = true;
                result.presenting = i;
            }
        }

        // We're done
        return result;
    }

    /* Examines whether or not the device supports the desired extensions. */
    bool check_device_extensions(VkPhysicalDevice gpu) {
        // Count the extensions available on the device
        uint32_t n_supported_extensions = 0;
        if (vkEnumerateDeviceExtensionProperties(gpu, nullptr, &n_supported_extensions, nullptr) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get supported extensions (count) from device.");
        }

        // Based on the count, get a list of those that are supported
        VkExtensionProperties supported_extensions[n_supported_extensions];
        if (vkEnumerateDeviceExtensionProperties(gpu, nullptr, &n_supported_extensions, supported_extensions) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get supported extensions from device.");
        }

        // Now check if all of our required extensions are supported
        for (size_t i = 0; i < this->device_extensions.size(); i++) {
            bool found = false;
            for (uint32_t j = 0; j < n_supported_extensions; j++) {
                if (strcmp(this->device_extensions[i], supported_extensions[j].extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            if (!found) { return false; }
        }
        return true;
    }

    /* Returns true if given GPU is suitable for our purposes, or false if it isn't. */
    bool is_gpu_suitable(VkPhysicalDevice gpu) {
        // Get the supported queues on this device
        DeviceQueueSupport queues = get_device_queues(gpu);

        // Check whether the device supports the correct extensions
        bool extensions_supported = check_device_extensions(gpu);

        // Check whether the device has a sufficiently supported swapchain
        bool swapchain_supported = false;
        if (extensions_supported) {
            // Get the swapchain support details on this device
            SwapChainSupport swapchain = get_swapchain_support(gpu);

            // Examine if they are enough
            swapchain_supported = !swapchain.formats.empty() && !swapchain.presentModes.empty();
        }

        // Return whether we support the GPU (we do, bc it's a tutorial)
        return queues.is_supported() && extensions_supported && swapchain_supported;
    }

    /* Browses and selects a physical device to run our Vulkan application on. */
    VkPhysicalDevice pick_gpu() {
        #ifdef DEBUG
        std::cout << INFO "Selecting GPU..." << std::endl;
        #endif

        // Start by getting the number of devices available
        uint32_t n_devices = 0;
        if (vkEnumeratePhysicalDevices(this->instance, &n_devices, nullptr) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Failed to get physical devices (count).");
        }

        // If we didn't find any, throw an error
        if (n_devices == 0) {
            throw std::runtime_error(ERROR "No Vulkan-supported GPUs found");
        }

        // Otherwise, go on to extact the device information list
        VkPhysicalDevice devices[n_devices];
        if (vkEnumeratePhysicalDevices(this->instance, &n_devices, devices) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Failed to get physical devices.");
        }

        // Find the first suitable device
        for (uint32_t i = 0; i < n_devices; i++) {
            if (is_gpu_suitable(devices[i])) {
                #ifdef DEBUG
                VkPhysicalDeviceProperties device_properties;
                vkGetPhysicalDeviceProperties(devices[i], &device_properties);
                std::cout << EMPTY "Selected GPU '" << device_properties.deviceName << "'" << std::endl;
                #endif
                return devices[i];
            }
        }

        // If no suitable ones are found, we're done here
        return nullptr;
    }



    /* Prepares the logical device we'll use. */
    VkDevice create_device(VkQueue* graphics_queue, VkQueue* present_queue) {
        #ifdef DEBUG
        std::cout << INFO "Creating logical device..." << std::endl;
        #endif

        // Get the queue indices supported by the device (which is guaranteed to be enough)
        DeviceQueueSupport device_queues = get_device_queues(this->gpu);
        size_t n_queues = 2;
        uint32_t queue_indices[] = { device_queues.graphics, device_queues.presenting };

        // If they are the same queue, merge them together and only define it as one
        if (queue_indices[0] == queue_indices[1]) {
            n_queues = 1;
        }

        // First, we'll define the queues we'll use for this logical device
        VkDeviceQueueCreateInfo queues[n_queues];
        for (size_t i = 0; i < n_queues; i++) {
            queues[i] = {};
            // Remember that we always need to specify the struct type
            queues[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            // Set the index to the graphics queue we found
            queues[i].queueFamilyIndex = queue_indices[i];
            // Set how many queues we'll want to create
            queues[i].queueCount = 1;
            // Set the priority of each queue (note that it's passed as a list)
            float queue_priority = 1.0f;
            queues[i].pQueuePriorities = &queue_priority;
        }

        // Next, define the specific set of feature's we'll require (empty for now)
        VkPhysicalDeviceFeatures device_features{};

        // Now we can start filling in the main createInfo for the logical device
        VkDeviceCreateInfo createInfo{};
        // Remember that we always need to specify the struct type
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // Specify that we only want n_queues queue families
        createInfo.queueCreateInfoCount = n_queues;
        // Add the queue info we created above
        createInfo.pQueueCreateInfos = queues;
        // Also pass the features to the createInfo struct
        createInfo.pEnabledFeatures = &device_features;
        // Mark the number of extensions we want to be enabled
        createInfo.enabledExtensionCount = static_cast<uint32_t>(this->device_extensions.size());
        // Mark the extensions themselves we want to be enabled
        createInfo.ppEnabledExtensionNames = device_extensions.data();
        #ifdef DEBUG
        // Set the device's validation layers (not really needed anymore, but for backwards capability)
        createInfo.enabledLayerCount = static_cast<uint32_t>(this->validation_layers.size());
        createInfo.ppEnabledLayerNames = this->validation_layers.data();
        #else
        // We're not using any layers
        createInfo.enabledLayerCount = 0;
        #endif

        // Armed with the prepared struct, it's time to initialize the device
        VkDevice result;
        if (vkCreateDevice(this->gpu, &createInfo, nullptr, &result) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not create the logical device.");
        }

        // Finally, set the queue handle before returning
        vkGetDeviceQueue(result, queue_indices[0], 0, graphics_queue);
        vkGetDeviceQueue(result, queue_indices[1], 0, present_queue);
        return result;
    }



    /* Function that chooses the correct format for the swapchain we'll use. */
    VkSurfaceFormatKHR swapchain_choose_format(const std::vector<VkSurfaceFormatKHR>& swapchain_formats) {
        // Loop through the formats and see if ours is supported
        for (size_t i = 0; i < swapchain_formats.size(); i++) {
            if (swapchain_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && swapchain_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                // We wanna have that one!
                return swapchain_formats[i];
            }
        }

        // Otherwise, let's just use the first available one (since we'll know there's at least one)
        return swapchain_formats[0];
    }

    /* Function that chooses the correct presentation mode for the swapchain we'll use. In particular, it's mostly how refreshrates work. */
    VkPresentModeKHR swapchain_choose_present_mode(const std::vector<VkPresentModeKHR>& swapchain_modes) {
        // Loop through the formats and see if ours is supported
        for (size_t i = 0; i < swapchain_modes.size(); i++) {
            // We ideally want triple buffering
            if (swapchain_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                // We wanna have that one!
                return swapchain_modes[i];
            }
        }

        // Otherwise, we'll go for the one that's guaranteed to be available (basically normal vsync)
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    /* Function that chooses the correct resolution for the swapchain we'll use. */
    VkExtent2D swapchain_choose_resolution(const VkSurfaceCapabilitiesKHR& capabilities) {
        // Distinguish between the special UINT32_MAX and just returning the extent in the capabilites (=resolution)
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        } else {
            // Get the current size of the window
            int width, height;
            glfwGetFramebufferSize(this->window, &width, &height);

            // Store them in a Vulkan extent
            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            // Next, update that extend with whichever is larger:
            //   - The minimum resolution that the swap chain supports
            //   - The smallest of either the maximum resolution supported by the swap chain or the width currently stored in it
            // In other words, make sure the resolution is bounded in between the minimum and maximum supported resolution
            if (actualExtent.width > capabilities.maxImageExtent.width) { actualExtent.width = capabilities.maxImageExtent.width; }
            if (actualExtent.width < capabilities.minImageExtent.width) { actualExtent.width = capabilities.minImageExtent.width; }
            if (actualExtent.height > capabilities.maxImageExtent.height) { actualExtent.height = capabilities.maxImageExtent.height; }
            if (actualExtent.height < capabilities.minImageExtent.height) { actualExtent.height = capabilities.minImageExtent.height; }

            // We're done here too
            return actualExtent;
        }
    }

    /* Creates the swapchain, using the properties as decided in the helper functions above. */
    VkSwapchainKHR create_swapchain(std::vector<VkImage>* swapchain_frames, VkFormat* swapchain_format, VkExtent2D* swapchain_extent) {
        // First, get the support details for the currently selected swapchain
        SwapChainSupport support = get_swapchain_support(this->gpu);

        // Based on that, select the configuration of the swapchain
        VkSurfaceFormatKHR format = swapchain_choose_format(support.formats);
        *swapchain_format = format.format;
        VkPresentModeKHR present_mode = swapchain_choose_present_mode(support.presentModes);
        *swapchain_extent = swapchain_choose_resolution(support.capabalities);

        // Also specify the maximum number of frames in the chain; we'll set it to the minimum number of images plus 1, unless it goes out-of-bounds
        uint32_t n_frames = support.capabalities.minImageCount + 1;
        // Note that 0 is escaped here, as it represents 'no maximum'
        if (support.capabalities.maxImageCount > 0 && n_frames > support.capabalities.maxImageCount) { n_frames = support.capabalities.maxImageCount; }

        // Armed with the configuration, we can now start constructing the createInfo struct for our swapchain
        VkSwapchainCreateInfoKHR createInfo{};
        // As always, pass the struct's type to the sType field
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        // Pass the surface used for this swapChain as well
        createInfo.surface = this->surface;
        // Next, tell it how many images it should have
        createInfo.minImageCount = n_frames;
        // Also pass the image format (just the format)
        createInfo.imageFormat = format.format;
        // And pass the image colourspace (also found in format)
        createInfo.imageColorSpace = format.colorSpace;
        // Set the extent of the swapchain
        createInfo.imageExtent = *swapchain_extent;
        // Always set to 1 layers, since we won't be creating a 3D-glasses application
        createInfo.imageArrayLayers = 1;
        // We'll use the image as a direct render target (other options might be to render it to memory first for post-processing)
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        // Next, we'll specify how to use the multiple queues we have defined
        DeviceQueueSupport queues = get_device_queues(this->gpu);
        if (queues.graphics != queues.presenting) {
            // Group the two indices into an array
            uint32_t indices[] = { queues.graphics, queues.presenting };

            // They are different queues, and thus we'll tell the swapchain to let both of them have ownership of the frames
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            // Tell the swapchain to share it between two queues...
            createInfo.queueFamilyIndexCount = 2;
            // ...which are:
            createInfo.pQueueFamilyIndices = indices;
        } else {
            // We can keep it simple, and tell the swapchain to only let the single queue be exclusive owner over the images
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            // We optionally tell it to not share
            createInfo.queueFamilyIndexCount = 0;
            // And optionally tell it there is no-one to share with
            createInfo.pQueueFamilyIndices = nullptr;
        }

        // We can specify we want a certain transformation in the swapchain (like rotating the image), but since we don't we'll just specify the current transform
        createInfo.preTransform = support.capabalities.currentTransform;
        // We can also tell the swapchain how to treat the opacity channel. In most cases, we'll just ignore it
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        // It's finally time to set the present mode
        createInfo.presentMode = present_mode;
        // If the cipped property is set to true, then the colours of pixels that are omitted by windows may be arbitrary (i.e., best performance)
        createInfo.clipped = VK_TRUE;
        // This can be used to re-optimise the swapchain, but for now we'll leave it be
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        // It's time to actually construct the VkSwapchain!
        VkSwapchainKHR result;
        if (vkCreateSwapchainKHR(this->device, &createInfo, nullptr, &result) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not create swapchain.");
        }

        // Get the handles from the freshly created swapchain. Note that the swapchain may have more images then we told it told (since we gave a minimum)
        if (vkGetSwapchainImagesKHR(this->device, result, &n_frames, nullptr) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get the swap chain frames (count) from the swapchain.");
        }
        swapchain_frames->resize(n_frames);
        if (vkGetSwapchainImagesKHR(this->device, result, &n_frames, swapchain_frames->data()) != VK_SUCCESS) {
            throw std::runtime_error(ERROR "Could not get the swap chain frames from the swapchain.");
        }
        
        // We're done creating the swapchain!
        return result;
    }



    /* Create the image views for every image in the swapchain. */
    void create_swapchain_views(std::vector<VkImageView>* swapchain_frameviews) {
        // Make sure there's enough space in the vector
        swapchain_frameviews->resize(this->swapchain_frames.size());

        // Create a view for each image
        for (size_t i = 0; i < this->swapchain_frames.size(); i++) {
            // As always, start with populating the required create info struct
            VkImageViewCreateInfo createInfo{};
            // Pass the struct's type to itself
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            // Pass a reference to the correct image to the construction struct
            createInfo.image = this->swapchain_frames[i];
            // Define how to interpret the images. Since it are output frames, they're 2D
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            // Set the image format to that of the swapchain
            createInfo.format = this->swapchain_format;
            // Something really sweet is that we can re-map colour channels (for example, to make the image monochrome). For now, though, we'll just use the standard mappings
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            // Next up, we'll tell the imageview what we'll do with the image; since it's output, there's nothing fancy like multiple levels or mipmaps
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            // Now, create the image view for this image
            if (vkCreateImageView(this->device, &createInfo, nullptr, &(*swapchain_frameviews)[i]) != VK_SUCCESS) {
                throw std::runtime_error(ERROR "Failed to create ImageView for frame " + std::to_string(i) + " in the swapchain.");
            }
        }
    }



    /* Handles initialization of Vulkan. */
    void init_vulkan(const std::vector<const char*>& required_extensions) {
        // First, initialize the library by creating an instance
        this->instance = this->create_vulkan_instance(required_extensions);

        #ifdef DEBUG
        // Once that's done, register the callback
        this->debug_messenger = this->init_vulkan_debug();
        #endif

        // Setup the surface we'll use
        this->surface = this->create_surface();

        // Be sure to pick a valid GPU with all of our requirements
        this->gpu = this->pick_gpu();
        if (this->gpu == nullptr) {
            throw std::runtime_error(ERROR "No suitable Vulkan-supported GPUs found");
        }
        this->device = this->create_device(&this->graphics_queue, &this->present_queue);

        // With the device ready, it's time to create a swapchain & its imageviews
        this->swapchain = this->create_swapchain(&this->swapchain_frames, &this->swapchain_format, &this->swapchain_extent);
        this->create_swapchain_views(&this->swapchain_frameviews);
    }

public:
    /* Width (in pixels) of the GLFW window. */
    const size_t wwidth;
    /* Height (in pixels) of the GLFW window. */
    const size_t wheight;

    /* Constructor for the HelloTriangleApplication class, which takes the size of the window and which validation layers to add in case DEBUG is defined. */
    HelloTriangleApplication(size_t width = 800, size_t height = 600, const std::vector<const char*>& device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }, const std::vector<const char*>& validation_layers = {"VK_LAYER_KHRONOS_validation"}) :
        device_extensions(device_extensions),
        validation_layers(validation_layers),
        wwidth(width),
        wheight(height)
    {
        // First, initialize the window
        this->init_window();

        // Then, get a list of all extensions we'll like to use
        std::vector<const char*> required_extensions = this->get_extensions();
        // Next, check if the required extensions are supported
        this->check_extensions(required_extensions);
        
        // Initialize vulkan with the required extensions
        this->init_vulkan(required_extensions);
    }

    /* Destructor for the HelloTriangleApplication. */
    ~HelloTriangleApplication() {
        #ifdef DEBUG
        std::cout << INFO "Cleaning up..." << std::endl;
        #endif

        // Since we explicitly created the imageviews, we also need to take them down
        for (size_t i = 0; i < this->swapchain_frameviews.size(); i++) {
            vkDestroyImageView(this->device, this->swapchain_frameviews[i], nullptr);
        }

        // Destroy the swapchain
        vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);

        // Be sure to destroy the logical device
        vkDestroyDevice(this->device, nullptr);
        
        // Destroy the Vulkan surface
        vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
        #ifdef DEBUG
        // First clean up the messenger
        DestroyDebugUtilsMessengerEXT(this->instance, this->debug_messenger, nullptr);
        #endif
        // Destroy the Vulkan instance
        vkDestroyInstance(this->instance, nullptr);

        // Be sure to cleanup the window and the library
        glfwDestroyWindow(this->window);
        glfwTerminate();
    }



    /* Main loop for the program. */
    void run() {
        #ifdef DEBUG
        std::cout << INFO "Entering main loop." << std::endl;
        #endif

        // Keep running until the window closed somehow
        while (!glfwWindowShouldClose(this->window)) {
            glfwPollEvents();
        }

        #ifdef DEBUG
        std::cout << INFO "Exiting main loop." << std::endl;
        #endif
    }

};


/* Entry point to the application. */
int main() {
    try {
        HelloTriangleApplication app;
        app.run();
    } catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << INFO "Done." << std::endl;
    return EXIT_SUCCESS;
}
