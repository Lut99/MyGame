/* COMMAND POOL.cpp
 *   by Lut99
 *
 * Created:
 *   14/01/2021, 17:01:05
 * Last edited:
 *   14/01/2021, 17:01:05
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The CommandPool class is a simple wrapper around the Vulkan
 *   VkCommandPool class, which is used to easily manage & register command
 *   buffers.
**/

#include "Debug/Debug.hpp"
#include "CommandPool.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** COMMANDBUFFER CLASS *****/
/* Constructor for the CommandBuffer class, which takes the command pool to create it in and the VkCommandBuffer to wrap. */
CommandBuffer::CommandBuffer(const CommandPool& command_pool, VkCommandBuffer command_buffer) :
    vk_command_buffer(command_buffer),
    command_pool(command_pool)
{}

/* Move constructor for the CommandBuffer class. */
CommandBuffer::CommandBuffer(CommandBuffer&& other) :
    vk_command_buffer(other.vk_command_buffer),
    command_pool(other.command_pool)
{
    other.vk_command_buffer = nullptr;
}

/* Destructor for the CommandBuffer class. */
CommandBuffer::~CommandBuffer() {
    DENTER("Vulkan::CommandBuffer::~CommandBuffer");
    DLOG(info, "Deallocating command buffer...");

    if (this->vk_command_buffer != nullptr) {
        vkFreeCommandBuffers(this->command_pool.device, this->command_pool, 1, &this->vk_command_buffer);
    }

    DLEAVE;
}



/* Begins recording the command buffer. Optionally takes any flags to start the command buffer with. */
void CommandBuffer::begin(VkCommandBufferUsageFlags flags) {
    DENTER("Vulkan::CommandBuffer::begin");

    // Start recording this command buffer right away
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    // Tell it we'll only be needin' the buffer once
    begin_info.flags = flags;
    // Start the record
    if (vkBeginCommandBuffer(this->vk_command_buffer, &begin_info) != VK_SUCCESS) {
        DLOG(fatal, "Could not begin recording the command buffer.");
    }

    DRETURN;
}

/* Stops recording the command buffer. */
void CommandBuffer::end() {
    DENTER("Vulkan::CommandBuffer::end");

    // Since that's all we wanna do, we're done here
    if (vkEndCommandBuffer(this->vk_command_buffer) != VK_SUCCESS) {
        DLOG(fatal, "Could not finish recording the command buffer.");
    }

    DRETURN;
}

/* Stops recording the command buffer and immediately submits it to the given VkQueue object. Only returns once the given queue is idle. */
void CommandBuffer::end(const VkQueue& queue) {
    DENTER("Vulkan::CommandBuffer::end(submit)");

    // Stop with recording the command buffer
    if (vkEndCommandBuffer(this->vk_command_buffer) != VK_SUCCESS) {
        DLOG(fatal, "Could not finish recording the command buffer.");
    }

    // Prepare the struct to submit the queue information
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    // Attach our internal buffer object
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &this->vk_command_buffer;

    // Submit to the queue and wait for it to finish
    if (vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE) != VK_SUCCESS) {
        DLOG(fatal, "Could not submit command buffer to the given queue.");
    }
    if (vkQueueWaitIdle(queue) != VK_SUCCESS) {
        DLOG(fatal, "Something went wrong while waiting for the given queue to be idle.");
    }

    DRETURN;
}





/***** COMMANDPOOL CLASS *****/
/* Constructor for the CommandPool class, which takes a device to schedule commands on, the queue index for which this command pool is responsible and optionally flags to use during creation. */
CommandPool::CommandPool(const Device& device, uint32_t queue_family, VkCommandPoolCreateFlags flags) :
    device(device),
    queue_family(queue_family)
{
    DENTER("Vulkan::CommandPool::CommandPool");
    DLOG(info, "Creating Vulkan command pool for queue " + std::to_string(queue_family) + "...");

    // Start by creating the obvious create_info
    VkCommandPoolCreateInfo command_pool_info{};
    command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    // Set the queue for this pool
    command_pool_info.queueFamilyIndex = queue_family;
    // Optionally provide some flags
    command_pool_info.flags = flags;

    // Create it
    if (vkCreateCommandPool(this->device, &command_pool_info, nullptr, &this->vk_command_pool) != VK_SUCCESS) {
        DLOG(fatal, "Could not create command pool for queue " + std::to_string(queue_family));
    }

    DLEAVE;
}

/* Move constructor for the CommandPool class. */
CommandPool::CommandPool(CommandPool&& other) :
    vk_command_pool(other.vk_command_pool),
    device(other.device),
    queue_family(other.queue_family)
{
    other.vk_command_pool = nullptr;
}

/* Destructor for the CommandPool class. */
CommandPool::~CommandPool() {
    DENTER("Vulkan::CommandPool::~CommandPool");
    DLOG(info, "Cleaning Vulkan command pool for queue " + std::to_string(queue_family) + "...");

    if (this->vk_command_pool != nullptr) {
        vkDestroyCommandPool(this->device, this->vk_command_pool, nullptr);
    }

    DLEAVE;
}



/* Returns a new command buffer at the given level. */
CommandBuffer CommandPool::get_buffer(VkCommandBufferLevel buffer_level) {
    DENTER("Vulkan::CommandPool::get_buffer");
    DLOG(info, "Allocating a command buffer...");

    // Prepare a command buffer
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    // Tell it that this is a primary command buffer, i.e., one that can be invoked by us instead of another command buffer
    alloc_info.level = buffer_level;
    // The command pool where the buffer will be created
    alloc_info.commandPool = this->vk_command_pool;
    // Tell it how many buffers to allocate
    alloc_info.commandBufferCount = 1;

    // Actually create it
    VkCommandBuffer command_buffer;
    if (vkAllocateCommandBuffers(this->device, &alloc_info, &command_buffer) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate temporary copy command buffer.");
    }

    // Wrap it in a CommandBuffer and we're done
    DRETURN CommandBuffer(*this, command_buffer);
}

/* Returns N new command buffers at the given level. */
Tools::Array<CommandBuffer> CommandPool::get_buffer(size_t N, VkCommandBufferLevel buffer_level) {
    DENTER("Vulkan::CommandPool::get_buffer(multiple)");
    DLOG(info, "Allocating " + std::to_string(N) + " command buffers...");

    // Prepare a command buffer
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    // Tell it that this is a primary command buffer, i.e., one that can be invoked by us instead of another command buffer
    alloc_info.level = buffer_level;
    // The command pool where the buffer will be created
    alloc_info.commandPool = this->vk_command_pool;
    // Tell it how many buffers to allocate
    alloc_info.commandBufferCount = static_cast<uint32_t>(N);

    // Actually create them
    VkCommandBuffer* command_buffer = new VkCommandBuffer[N];
    if (vkAllocateCommandBuffers(this->device, &alloc_info, command_buffer) != VK_SUCCESS) {
        DLOG(fatal, "Could not allocate temporary copy command buffer.");
    }

    // Wrap each of them in an array of command buffers
    Tools::Array<CommandBuffer> result(N);
    for (size_t i = 0; i < N; i++) {
        result.push_back(CommandBuffer(*this, command_buffer[i]));
    }

    // Delete the array of VkCommandBuffer pointers and we're done
    delete[] command_buffer;
    DRETURN result;
}
