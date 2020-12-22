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
        GLFWwindow* window;

        /* Indicates if the window recently resized or not. */
        bool did_resize;

        /* Function that is called when the window resizes. */
        static void GLFW_resize_callback(GLFWwindow* window, int new_width, int new_height);

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
        inline bool done() const { return (bool) glfwWindowShouldClose(this->window); }

        /* Returns whether or not the window recently resized. */
        inline bool resized() const { return this->did_resize; }
        /* Resets the status of window resize back to false. */
        inline void reset_resized() { this->did_resize = false; }

    };
}

#endif
