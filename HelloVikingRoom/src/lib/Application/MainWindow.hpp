/* MAIN WINDOW.hpp
 *   by Lut99
 *
 * Created:
 *   21/12/2020, 13:02:35
 * Last edited:
 *   21/12/2020, 13:02:35
 * Auto updated?
 *   Yes
 *
 * Description:
 *   The MainWindow class is used to wrap both the required window and the
 *   required Vulkan structures and code that initializes, maintains and
 *   destructs the window.
**/

#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#define GLFW_DLL
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

#include "Vulkan/Instance.hpp"

namespace HelloVikingRoom {
    /* Class that creates, maintains and destroys the window & associated vulkan structures. */
    class MainWindow {
    private:
        /* The Vulkan instance that this window is based on. */
        const Vulkan::Instance& instance;
        /* The GLFW window object that this class wraps. */
        GLFWwindow* glfw_window;
        /* The VkSurface object that this class also wraps. */
        VkSurfaceKHR vk_surface;

        /* Indicates if the window recently resized or not. */
        bool did_resize;
        /* Indicates if left has been pressed since last reset() call. */
        bool left_down;
        /* Indicates if a has been pressed since last reset() call. */
        bool a_down;
        /* Indicates if right has been pressed since last reset() call. */
        bool right_down;
        /* Indicates if d has been pressed since last reset() call. */
        bool d_down;

        /* Function that is called when the window resizes. */
        static void GLFW_resize_callback(GLFWwindow* window, int new_width, int new_height);
        /* Function that is called whenever a key is pressed/released. */
        static void GLFW_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    public:
        /* The title of the window. */
        const std::string title;
        /* The width of the window (in pixels). */
        int width;
        /* The height of the window (in pixels). */
        int height;


        /* Constructor for the MainWindow class, which takes a vulkan instance, the title of the window and its size (in pixels). */
        MainWindow(const Vulkan::Instance& instance, const std::string& title, int width, int height);
        /* The copy constructor for the MainWindow class is deleted, as it makes no sense to copy a handle. */
        MainWindow(const MainWindow& other) = delete;
        /* Move constructor for the MainWindow class. */
        MainWindow(MainWindow&& other);
        /* Destructor for the MainWindow class. */
        ~MainWindow();

        /* Runs the events for the internal window. */
        void do_events();
        /* Returns if the window is done with events or not. */
        inline bool done() const { return (bool) glfwWindowShouldClose(this->glfw_window); }

        /* Returns whether or not the window recently resized. */
        inline bool resized() const { return this->did_resize; }
        /* Returns whether or not left/a was pressed since the last status update. */
        inline bool left_pressed() const { return this->left_down | this->a_down; }
        /* Returns whether or not right/d was pressed since the last status update. */
        inline bool right_pressed() const { return this->right_down | this->d_down; }
        /* Resets the status of window resize back to false. */
        inline void reset_resized() { this->did_resize = false; }

        /* Explicitly returns the internal window object, if necessary. */
        inline GLFWwindow* const window() const { return this->glfw_window; }
        /* Implicitly returns the internal window object. */
        inline operator GLFWwindow*() const { return this->glfw_window; }
        /* Explicitly returns the internal surface object, if necessary. */
        inline const VkSurfaceKHR surface() const { return this->vk_surface; }
        /* Implicitly returns the internal surface object. */
        inline operator VkSurfaceKHR() const { return this->vk_surface; }

    };
}

#endif
