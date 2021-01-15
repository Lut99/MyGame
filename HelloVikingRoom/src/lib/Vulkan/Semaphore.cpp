/* SEMAPHORE.cpp
 *   by Lut99
 *
 * Created:
 *   15/01/2021, 14:54:59
 * Last edited:
 *   15/01/2021, 14:54:59
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Semaphore class, which wraps a VkSemaphore object. These
 *   objects are meant to synchronize between command queue's in in them,
 *   and so that's also the function of this class.
**/

#include "Debug/Debug.hpp"
#include "Semaphore.hpp"

using namespace std;
using namespace HelloVikingRoom::Vulkan;
using namespace Debug::SeverityValues;


/***** SEMAPHORE CLASS *****/
/* Constructor for the Semaphore class, which only takes the device where the semaphore will function on and live. */
Semaphore::Semaphore(const Device& device) :
    vk_semaphore(nullptr),
    device(device)
{
    DENTER("Vulkan::Semaphore::Semaphore");
    DLOG(info, "Intializing Vulkan semaphore...");

    // Create the creation struct, which doesn't do anything now but might in the future
    this->vk_semaphore_info = {};
    this->vk_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    // Create the semaphore by calling our internal reset() function
    this->reset();

    DLEAVE;
}

/* Move constructor for the Semaphore class. */
Semaphore::Semaphore(Semaphore&& other) :
    vk_semaphore(other.vk_semaphore),
    vk_semaphore_info(other.vk_semaphore_info),
    device(other.device)
{
    other.vk_semaphore = nullptr;
}

/* Destructor for the Semaphore class. */
Semaphore::~Semaphore() {
    DENTER("Vulkan::Semaphore::~Semaphore");
    DLOG(info, "Cleaning Vulkan semaphore...");

    if (this->vk_semaphore != nullptr) {
        vkDestroySemaphore(this->device, this->vk_semaphore, nullptr);
    }

    DLEAVE;
}



/* Resets the semaphore by destroying and then re-creating the internal VkSemaphore object. */
void Semaphore::reset() {
    DENTER("Vulkan::Semaphore::reset");

    // Destroy the old semaphore if there is one
    if (this->vk_semaphore != nullptr) {
        vkDestroySemaphore(this->device, this->vk_semaphore, nullptr);
    }

    // Create the new semaphore
    if (vkCreateSemaphore(this->device, &this->vk_semaphore_info, nullptr, &this->vk_semaphore) != VK_SUCCESS) {
        DLOG(fatal, "Could not create the semaphore.");
    }

    DRETURN;
}
