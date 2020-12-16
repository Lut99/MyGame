/* MAIN.cpp
 *   by Lut99
 *
 * Created:
 *   16/12/2020, 13:34:59
 * Last edited:
 *   16/12/2020, 16:30:12
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
    std::cerr << VULKAN << log_prefix << pCallbackData->pMessage << " (" << log_type << ")" << std::endl;
    return VK_FALSE;
}
#endif




/***** HELPER STRUCTS *****/
/* Struct that is used to examine the queues present on a given device. */
struct DeviceQueueSupport {
    /* Marks whether or not the device supports graphics capabilities. */
    bool supports_graphics;

    /* If it has a value, then the device supports graphics queues. */
    uint32_t graphics;

    /* Returns true if all interesting queues have been found, or false otherwise. */
    inline bool is_supported() const { return supports_graphics; }
};





/***** MAIN CLASS *****/
/* Class that wraps our application. */
class HelloTriangleApplication {
private:
    /* The Window */
    GLFWwindow* window;
    /* The Vulkan instance */
    VkInstance instance;
    /* Validation layers enabled when the DEBUG-flag is specified. */
    std::vector<const char*> validation_layers;
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
        vkEnumerateInstanceExtensionProperties(nullptr, &n_existing_extensions, nullptr);
        VkExtensionProperties existing_extensions[n_existing_extensions];
        vkEnumerateInstanceExtensionProperties(nullptr, &n_existing_extensions, existing_extensions);

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
        vkEnumerateInstanceLayerProperties(&n_layers, nullptr);

        // Extend that to get layer information
        VkLayerProperties existing_layers[n_layers];
        vkEnumerateInstanceLayerProperties(&n_layers, existing_layers);

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
        }

        // We're done
        return result;
    }

    /* Returns true if given GPU is suitable for our purposes, or false if it isn't. */
    bool is_gpu_suitable(VkPhysicalDevice gpu) {
        // Get the physical properties of the device
        VkPhysicalDeviceProperties device_properties;
        vkGetPhysicalDeviceProperties(gpu, &device_properties);
        (void) device_properties;

        // Also get its features
        VkPhysicalDeviceFeatures device_features;
        vkGetPhysicalDeviceFeatures(gpu, &device_features);
        (void) device_features;

        // Return whether we support the GPU (we do, bc it's a tutorial)
        return get_device_queues(gpu).is_supported();
    }

    /* Browses and selects a physical device to run our Vulkan application on. */
    VkPhysicalDevice pick_gpu() {
        #ifdef DEBUG
        std::cout << INFO "Selecting GPU..." << std::endl;
        #endif

        // Start by getting the number of devices available
        uint32_t n_devices = 0;
        vkEnumeratePhysicalDevices(this->instance, &n_devices, nullptr);

        // If we didn't find any, throw an error
        if (n_devices == 0) {
            throw std::runtime_error(ERROR "No Vulkan-supported GPUs found");
        }

        // Otherwise, go on to extact the device information list
        VkPhysicalDevice devices[n_devices];
        vkEnumeratePhysicalDevices(this->instance, &n_devices, devices);

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
    VkDevice create_device(VkQueue* graphics_queue) {
        #ifdef DEBUG
        std::cout << INFO "Creating logical device..." << std::endl;
        #endif

        // First, we'll define the queues we'll use for this logical device
        DeviceQueueSupport queues = get_device_queues(this->gpu);
        VkDeviceQueueCreateInfo queue_createInfo{};
        // Remember that we always need to specify the struct type
        queue_createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        // Set the index to the graphics queue we found
        queue_createInfo.queueFamilyIndex = queues.graphics;
        // Set how many queues we'll want to create
        queue_createInfo.queueCount = 1;
        // Set the priority of each queue (note that it's passed as a list)
        float queue_priority = 1.0f;
        queue_createInfo.pQueuePriorities = &queue_priority;

        // Next, define the specific set of feature's we'll require (empty for now)
        VkPhysicalDeviceFeatures device_features{};

        // Now we can start filling in the main createInfo for the logical device
        VkDeviceCreateInfo createInfo{};
        // Remember that we always need to specify the struct type
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        // Add the queue info we created above
        createInfo.pQueueCreateInfos = &queue_createInfo;
        // Specify that we only want one queue family
        createInfo.queueCreateInfoCount = 1;
        // Also pass the features to the createInfo struct
        createInfo.pEnabledFeatures = &device_features;
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
        vkGetDeviceQueue(result, queues.graphics, 0, graphics_queue);
        return result;
    }



    /* Handles initialization of Vulkan. */
    void init_vulkan(const std::vector<const char*>& required_extensions) {
        // First, initialize the library by creating an instance
        this->instance = this->create_vulkan_instance(required_extensions);

        #ifdef DEBUG
        // Once that's done, register the callback
        this->debug_messenger = this->init_vulkan_debug();
        #endif

        // Be sure to pick a valid GPU with all of our requirements
        this->gpu = this->pick_gpu();
        if (this->gpu == nullptr) {
            throw std::runtime_error(ERROR "No suitable Vulkan-supported GPUs found");
        }
        this->device = this->create_device(&this->graphics_queue);
    }

public:
    /* Width (in pixels) of the GLFW window. */
    const size_t wwidth;
    /* Height (in pixels) of the GLFW window. */
    const size_t wheight;

    /* Constructor for the HelloTriangleApplication class, which takes the size of the window and which validation layers to add in case DEBUG is defined. */
    HelloTriangleApplication(size_t width = 800, size_t height = 600, const std::vector<const char*>& validation_layers = {"VK_LAYER_KHRONOS_validation"}) :
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

        // Be sure to destroy the logical device first
        vkDestroyDevice(this->device, nullptr);
        
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
    return EXIT_SUCCESS;
}
