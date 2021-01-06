/* DEBUG.hpp
 *   by Lut99
 *
 * Created:
 *   06/01/2021, 13:58:27
 * Last edited:
 *   06/01/2021, 13:58:27
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Second take at writing a useful debugger. This one allows functions to
 *   define tasks, which should streamline formatting in a very good way.
**/

#ifndef DEBUG_HPP
#define DEBUG_HPP

/***** IMPORTS *****/
#include <iostream>
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>
#include <thread>





/***** CONSTANTS *****/
/* The colour of information messages (i.e., task-less messages). */
#define INFO_MAKEUP "\033[36;1m"
/* The colour of warning messages. */
#define WARNING_MAKEUP "\033[33m"
/* The colour of errors. */
#define ERROR_MAKEUP "\033[31;1m"
/* The colour of tasks that are still busy. */
#define BUSY_MAKEUP "\033[1m"
/* The colour of tasks that are successfull. */
#define SUCCESS_MAKEUP "\033[32;1m"
/* The colour of tasks that have failed. */
#define FAILURE_MAKEUP "\033[31;1;7m"
/* The makeup of vulkan warning messages. */
#define VULKAN_WARNING_MAKEUP WARNING_MAKEUP
/* The makeup of vulkan error messages. */
#define VULKAN_ERROR_MAKEUP ERROR_MAKEUP
/* Resets the terminal again. */
#define RESET_MAKEUP "\033[0m"





/***** HELPER MACROS *****/
#ifndef NDEBUG

/* Adds the current function to the call stack under the given name. */
#define DENTER(FUNC_NAME) \
    Debug::debugger.push((FUNC_NAME), (__FILE__), (__LINE__) - 1);
/* Wraps the return statement, first popping the current value from the stack. */
#define DRETURN \
    Debug::debugger.pop(); \
    return
/* Pops the current frame from the call stack only, but does not call return. */
#define DLEAVE \
    Debug::debugger.pop();

/* Logs a message to the Debugger. */
#define DLOG(SEVERITY, MESSAGE) \
    Debug::debugger.log((SEVERITY), (MESSAGE));

/* Starts a new task in the Debugger with the given name & description. */
#define DSTART(TASK_NAME, TASK_MESSAGE) \
    Debug::debugger.start_task((TASK_NAME), (TASK_MESSAGE));
/* Finishes a task (and all its subtasks) successfully. */
#define DSTOP(TASK_NAME) \
    Debug::debugger.end_task((TASK_NAME));
/* Finishes a task (and all its subtasks) unsuccessfully. */
#define DFAIL(TASK_NAME) \
    Debug::debugger.fail_task((TASK_NAME));

#else

/* Adds the current function to the call stack under the given name. */
#define DENTER(FUNC_NAME)
/* Wraps the return statement, first popping the current value from the stack. */
#define DRETURN \
    return
/* Pops the current frame from the call stack only, but does not call return. */
#define DLEAVE

/* Logs a message to the Debugger. */
#define DLOG(SEVERITY, MESSAGE) \
    if ((SEVERITY) == Debug::Severity::nonfatal) { \
        std::cerr << "NONFATAL ERROR: " << (MESSAGE) << std::endl; \
    } else if ((SEVERITY) == Debug::Severity::vulkan_error) { \
        std::cerr << "VULKAN ERROR: " << (MESSAGE) << std::endl; \
    } else if ((SEVERITY) == Debug::Severity::fatal) { \
        std::cerr << "FATAL ERROR: " << (MESSAGE) << std::endl; \
        exit(EXIT_FAILURE); \
    }

/* Starts a new task in the Debugger with the given name & description. */
#define DSTART(TASK_NAME, TASK_MESSAGE)
/* Finishes a task (and all its subtasks) successfully. */
#define DSTOP(TASK_NAME)
/* Finishes a task (and all its subtasks) unsuccessfully. */
#define DFAIL(TASK_NAME)

#endif





/***** DEBUG CLASS CODE *****/
namespace Debug {
    /* Enum that defines the possible debug message types. */
    namespace SeverityValues {
        enum type {
            // Only provides the necessary indents, but does not print a message.
            auxillary,
            // Prints a message that just tells something to the user
            info,
            // Prints a message that tells the user to change something, but that isn't necessary to change
            warning,
            // Prints a message that tells the user that an error occurred but that execution could continue
            nonfatal,
            // Prints a message that tells the user an error occurred, and stops execution of the program
            fatal,

            // Logs a Vulkan warning message to the terminal
            vulkan_warning,
            // Logs a Vulkan error message to the terminal
            vulkan_error
        };
    }
    /* Enum that defines the possible debug message types. */
    using Severity = SeverityValues::type;



    /* Struct used to refer to a stack frame. */
    struct Frame {
        /* The name of the function we entered. */
        std::string func_name;
        /* The file where the function resides. */
        std::string file_name;
        /* The line number where the function is defined (i.e., the line above DENTER). */
        size_t line_number;
    };

    /* Struct used to describe a single task. */
    class Task {
    public:
        /* The name of the task. */
        std::string name;
        /* The message shown on the terminal for this task. Already includes newlines and indents. */
        std::string message;
        /* The number of lines that this task consists of. */
        size_t n_lines;

        /* Constructor for the Task class, which also computes the lines to print. */
        Task(const std::string& name, const std::string& message);
    };



    /* The Debugger class, which is the debugger that handles the function stack and logging. */
    class Debugger {
    public:
        /* The stream where the Debugger works on. */
        static constexpr std::ostream& const os = std::cerr;
        /* The total line width that the Debugger uses. */
        static constexpr size_t line_width = 100;
        /* The size of each prefix message. */
        static constexpr size_t prefix_size = 10;
        /* The extra spaces each indent adds. */
        static constexpr size_t indent_size = 3;

    private:
        /* The callstack for each thread. */
        std::unordered_map<std::thread::id, std::vector<Frame>> call_stack;
        /* The tasks currently scheduled per thread. */
        std::unordered_map<std::thread::id, std::vector<Task>> tasks;
        /* The order in which tasks where scheduled. */
        std::vector<std::string> tasks_order;

        /* Keeps track of whether or not we should use ansi commands. */
        bool ansi_support;
        /* Keeps track of whether or not the Debugger has been called from multiple threads. If so, starts using locks for everything. */
        bool used_multithreaded;
        /* The initial thread that the Debugger was created. Used to check if other threads use this instance as well. */
        std::thread::id first_thread;

        /* Prints a given string over multiple lines, pasting n spaces in front of each one and linewrapping on the target width. Optionally, a starting x can be specified. */
        void _print_linewrapped(size_t& x, size_t indent, const std::string& message);
        /* Clears the lines of the currently busy tabs. */
        void _clear();
        /* Writes the lines of the currently busy tabs. */
        void _write();

    public:
        /* Default constructor for the Debugger class. Optionally overrides the initial thread safety feature. */
        Debugger(bool multithreaded = false);

        /* Enters a new function, popping it's value on the stack. */
        void push(const std::string& function_name, const std::string& file_name, size_t line_number);
        /* pops the top function name of the stack. */
        void pop();

        /* Logs a message to stdout. The type of message must be specified, which also determines how the message will be printed. If the the severity is fatal, also throws a std::runtime_error with the same text. To disable that, use Severity::nonfatal otherwise. Finally, one can optionally specify extra levels of indentation to use for this message. */
        void log(Severity severity, const std::string& message);

        /* Initializes a new task. Note that if there are already tasks on the stack, then this task is considered a subtask of the previous one. */
        void start_task(const std::string& task_name, const std::string& task_message);
        /* Finalizes an existing task as if it were successfull. Any tasks consider subtasks of this one will be closed with the same status. */
        void end_task(const std::string& task_name);
        /* Finalizes an existing task as if it failed. Any tasks consider subtasks of this one will be closed with the same status. */
        void fail_task(const std::string& task_name);
    };



    #ifndef NDEBUG
    /* Debugging instance that is shared across all files. */
    extern Debugger debugger;
    #endif
}

#endif
