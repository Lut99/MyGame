/* DEBUG.cpp
 *   by Lut99
 *
 * Created:
 *   06/01/2021, 13:58:22
 * Last edited:
 *   06/01/2021, 13:58:22
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Second take at writing a useful debugger. This one allows functions to
 *   define tasks, which should streamline formatting in a very good way.
**/

#include <sstream>
#include <stdexcept>
#ifdef _WIN32
#include <windows.h>
#endif

#include "Debug.hpp"

using namespace std;
using namespace Debug;
using namespace Debug::SeverityValues;


/***** GLOBALS *****/
#ifndef NDEBUG
/* Debugging instance that is shared across all files. */
Debugger Debug::debugger;
#endif





/***** TASK CLASS *****/
/* Constructor for the Task class, which also computes the lines to print. */
Task::Task(const std::string& name, const std::string& message) :
    name(name),
    n_lines(1)
{
    // Loop to find the message
    size_t max_width = Debugger::line_width - Debugger::prefix_size;
    std::stringstream sstr;
    for (size_t i = 0; i < message.size(); i++) {
        if (i < message.size() - 1 && i % max_width == max_width - 1) {
            sstr << endl << string(Debugger::prefix_size, ' ');
            ++this->n_lines;
        }
        sstr << message[i];
    }

    // Store it
    this->message = sstr.str();
}





/***** DEBUGGER CLASS *****/
/* Default constructor for the Debugger class. Optionally overrides the initial thread safety feature. */
Debugger::Debugger(bool multithreaded) :
    used_multithreaded(multithreaded),
    first_thread(this_thread::get_id())
{
    // Find out if we support colours
    #ifdef _WIN32
    // For Windows, we check
    DWORD modes;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &modes);
    this->ansi_support = modes & ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    #else
    // Let's assume the rest does
    this->ansi_support = true;
    #endif
}



/* Prints a given string over multiple lines, pasting n spaces in front of each one and linewrapping on the target width. Optionally, a starting x can be specified. */
void Debugger::_print_linewrapped(size_t& x, size_t indent, const std::string& message) {
    // Get the string to be pasted in front of every new line
    size_t prefix_size = Debugger::prefix_size + Debugger::indent_size * indent;
    std::string prefix = std::string(prefix_size, ' ');
    // Loop to print each character
    bool ignoring = false;
    for (size_t i = 0; i < message.size(); i++) {
        // If we're seeing a '\033', ignore until an 'm' is reached
        if (!ignoring && message[i] == '\033') { ignoring = true; }
        else if (ignoring && message[i] == 'm') { ignoring = false; }

        // Otherwise, check if we should print a newline (only when we're not printing color codes)
        if (!ignoring && ++x >= Debugger::line_width - prefix_size) {
            Debugger::os << std::endl << prefix;
            x = 0;
        }

        // Print the character itself
        Debugger::os << message[i];
    }
}

/* Clears the lines of the currently busy tasks. */
void Debugger::_clear() {
    // For all tasks, remove the required number of lines
}

/* Writes the lines of the currently busy tasks. */
void Debugger::_write() {

}



/* Enters a new function, popping it's value on the stack. */
void Debugger::push(const std::string& function_name, const std::string& file_name, size_t line_number) {
    // Create a new stack frame
    Frame frame({ function_name, file_name, line_number });

    // Push it on the stack
    this->call_stack.at(this_thread::get_id()).push_back(frame);
}

/* pops the top function name of the stack. */
void Debugger::pop() {
    // Only pop if there are elements left
    std::vector<Frame>& call_stack = this->call_stack.at(this_thread::get_id());
    if (call_stack.size() > 0) {
        call_stack.pop_back();
    }
}



/* Logs a message to stdout. The type of message must be specified, which also determines how the message will be printed. If the the severity is fatal, also throws a std::runtime_error with the same text. To disable that, use Severity::nonfatal otherwise. Finally, one can optionally specify extra levels of indentation to use for this message. */
void Debugger::log(Severity severity, const std::string& message) {
    
}



/* Initializes a new task. Note that if there are already tasks on the stack, then this task is considered a subtask of the previous one. */
void Debugger::start_task(const std::string& task_name, const std::string& task_message) {
    // Check if the task already exists
    vector<Task>& tasks = this->tasks.at(this_thread::get_id());
    for (size_t i = 0; i < tasks.size(); i++) {
        if (tasks[i].name == task_name) {
            throw std::runtime_error("META ERROR: Duplicate task '" + task_name + "' specified.");
        }
    }

    // Add it if not
    this->_clear();
    tasks.push_back(Task({ task_name, task_message }));
    this->_write();
}

/* Finalizes an existing task as if it were successfull. Any tasks consider subtasks of this one will be closed with the same status. */
void Debugger::end_task(const std::string& task_name) {
    // Check if the task actually exists
    vector<Task>& tasks = this->tasks.at(this_thread::get_id());
    for (vector<Task>::const_iterator iter = tasks.begin(); iter != tasks.end(); ++iter) {
        if ((*iter).name == task_name) {
            // Update the terminal that it is done
            this->_clear();
            if (this->ansi_support) {
                Debugger::os << "[" SUCCESS_MAKEUP "SUCCESS" RESET_MAKEUP "] ";
            } else {
                Debugger::os << "[SUCCESS] ";
            }
            (*iter).print_message(Debugger::os);

            // Remove the task, then write them
            tasks.erase(iter);
            this->_write();
            return;
        }
    }

    // Otherwise, not found
    throw std::runtime_error("META ERROR: Cannot stop non-existent task '" + task_name + "'.");
}

/* Finalizes an existing task as if it failed. Any tasks consider subtasks of this one will be closed with the same status. */
void Debugger::fail_task(const std::string& task_name) {
    // Check if the task actually exists
    vector<Task>& tasks = this->tasks.at(this_thread::get_id());
    for (vector<Task>::const_iterator iter = tasks.begin(); iter != tasks.end(); ++iter) {
        if ((*iter).name == task_name) {
            // Update the terminal that it is done
            this->_clear();
            if (this->ansi_support) {
                Debugger::os << "[" FAILURE_MAKEUP "FAILURE" RESET_MAKEUP "] ";
            } else {
                Debugger::os << "[FAILURE] ";
            }
            (*iter).print_message(Debugger::os);

            // Remove the task, then write them
            tasks.erase(iter);
            this->_write();
            return;
        }
    }

    // Otherwise, not found
    throw std::runtime_error("META ERROR: Cannot fail non-existent task '" + task_name + "'.");
}
