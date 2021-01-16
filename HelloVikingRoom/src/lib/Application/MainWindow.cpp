/* MAIN WINDOW.cpp
 *   by Lut99
 *
 * Created:
 *   21/12/2020, 13:02:31
 * Last edited:
 *   12/24/2020, 12:58:22 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The MainWindow class is used to wrap both the required window and the
 *   required Vulkan structures and code that initializes, maintains and
 *   destructs the window.
**/

#include "Debug/Debug.hpp"
#include "MainWindow.hpp"

using namespace std;
using namespace HelloVikingRoom;
using namespace Debug::SeverityValues;


/***** MAINWINDOW CLASS *****/
/* Constructor for the MainWindow class, which takes a vulkan instance, the title of the window and its size (in pixels). */
MainWindow::MainWindow(const Vulkan::Instance& instance, const std::string& title, int width, int height) :
    instance(instance),
    left_down(false),
    a_down(false),
    right_down(false),
    d_down(false),
    title(title),
    width(width),
    height(height)
{
    DENTER("MainWindow::MainWindow");
    DLOG(info, "Initializing window...");

    // Next, tell the API that we'll create a non-OpenGL window. */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // Next, create the window itself
    this->glfw_window = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);
    if (this->glfw_window == nullptr) {
        DLOG(fatal, "Could not initialize GLFW window");
    }

    // Register the resize & mouse position callbacks for this window
    glfwSetWindowUserPointer(this->glfw_window, (void*) this);
    glfwSetFramebufferSizeCallback(this->glfw_window, MainWindow::GLFW_resize_callback);
    glfwSetKeyCallback(this->glfw_window, MainWindow::GLFW_key_callback);



    // Move on to initializing the surface
    DLOG(auxillary, "Initializing surface...");

    // Simply call glfw's function
    if (glfwCreateWindowSurface(this->instance, this->glfw_window, nullptr, &this->vk_surface) != VK_SUCCESS) {
        DLOG(fatal, "Could not create window surface");
    }

    // Done!
    DLEAVE;
}

/* Move constructor for the MainWindow class. */
MainWindow::MainWindow(MainWindow&& other) :
    instance(other.instance),
    glfw_window(other.glfw_window),
    vk_surface(other.vk_surface),
    did_resize(other.did_resize),
    left_down(other.left_down),
    a_down(other.a_down),
    right_down(other.right_down),
    d_down(other.d_down),
    title(other.title),
    width(other.width),
    height(other.height)
{
    // Set the other's surface etc to nullptr's
    other.glfw_window = nullptr;
    other.vk_surface = nullptr;
}

/* Destructor for the MainWindow class. */
MainWindow::~MainWindow() {
    DENTER("MainWindow::~MainWindow");
    DLOG(info, "Cleaning MainWindow...");

    if (this->glfw_window != nullptr) {
        // Destroy the wrapped window object
        glfwDestroyWindow(this->glfw_window);
    }
    if (this->vk_surface != nullptr) {
        // Destroy the surface
        vkDestroySurfaceKHR(this->instance, this->vk_surface, nullptr);
    }

    DLEAVE;
}



/* Function that is called when the window resizes. */
void MainWindow::GLFW_resize_callback(GLFWwindow* window, int new_width, int new_height){
    DENTER("MainWindow::GLFW_resize_callback");

    // Get a reference to our class
    MainWindow* main_window = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    // Update the new width and height in the application
    main_window->width = new_width;
    main_window->height = new_height;
    // Also actually mark that we resized
    main_window->did_resize = true;

    // We're done
    DRETURN;
}

/* Function that is called whenever a key is pressed/released. */
void MainWindow::GLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    DENTER("MainWindow::GLFW_key_callback");

    // Get a reference to our class
    MainWindow* main_window = reinterpret_cast<MainWindow*>(glfwGetWindowUserPointer(window));

    // Do only events we like
    if (action != GLFW_PRESS && action != GLFW_RELEASE) { DRETURN; }
    // But do remember which
    bool new_state = action == GLFW_PRESS;

    // Check the keys we're interested in
    switch(key) {
        case GLFW_KEY_LEFT:
            main_window->left_down = new_state;
            DRETURN;

        case GLFW_KEY_A:
            main_window->a_down = new_state;
            DRETURN;
        case GLFW_KEY_RIGHT:
            main_window->right_down = new_state;
            DRETURN;
            
        case GLFW_KEY_D:
            main_window->d_down = new_state;
            DRETURN;
            
        default:
            // Do nothing
            DRETURN;
            
    }
}



/* Runs the events for the internal window. */
void MainWindow::do_events() {
    // Be sure to do the events of this (and, therefore, all) windows
    glfwPollEvents();
}
