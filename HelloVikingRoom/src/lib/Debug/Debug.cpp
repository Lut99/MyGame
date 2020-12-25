/* DEBUG.cpp
 *   by Lut99
 *
 * Created:
 *   19/12/2020, 16:32:34
 * Last edited:
 *   19/12/2020, 16:32:34
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This file contains a more advanced method of debugging, where we can
 *   specify the debugging type and where its timestamp is noted.
 *   Aditionally, lines are automatically linewrapped (with correct
 *   indents), and extra indentation levels can be given based on functions
 *   entered or left.
**/

#include <algorithm>
#include <stdexcept>
#ifdef _WIN32
#include "windows.h"
#endif

#include "Debug.hpp"

using namespace Debug;


/***** GLOBALS *****/
/* Global instance of the debug class, used for debugging. */
Debugger Debug::debugger;





/***** HELPER FUNCTIONS *****/
/* Returns whether or not the associated terminal supports ANSI color codes. */
static bool terminal_supports_colours() {
    #ifdef _WIN32
    // For Windows, we check
    DWORD modes;
    GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &modes);
    return modes & ENABLE_VIRTUAL_TERMINAL_PROCESSING;

    #else
    // Let's assume the rest does
    return true;
    #endif
}





/***** DEBUGGER CLASS *****/
/* Default constructor for the Debugger class. */
Debugger::Debugger() :
    colour_enabled(terminal_supports_colours()),
    auxillary_msg("       "),
    info_msg(this->colour_enabled ? "[" GREEN " OK " RESET "] " : "[ OK ] "),
    warning_msg(this->colour_enabled ? "[" YELLOW "WARN" RESET "] " : "[WARN] "),
    nonfatal_msg(this->colour_enabled ? "[" RED "FAIL" RESET "] " : "[FAIL] "),
    fatal_msg(this->colour_enabled ? "[" RED REVERSED "ERRR" RESET "] " : "[ERRR] "),
    reset_msg(this->colour_enabled ? RESET : "")
{}



/* Prints a given string over multiple lines, pasting n spaces in front of each one and linewrapping on the target width. Optionally, a starting x can be specified. */
void Debugger::print_linewrapped(std::ostream& os, size_t& x, size_t width, const std::string& message) {
    // Get the string to be pasted in front of every new line
    std::string prefix = std::string(7 + this->indent_level * 7, ' ');
    // Loop to print each character
    bool ignoring = false;
    for (size_t i = 0; i < message.size(); i++) {
        // If we're seeing a '\033', ignore until an 'm' is reached
        if (!ignoring && message[i] == '\033') { ignoring = true; }
        else if (ignoring && message[i] == 'm') { ignoring = false; }

        // Otherwise, check if we should print a newline (only when we're not printing color codes)
        if (!ignoring && ++x >= width) {
            os << std::endl << prefix;
            x = 0;
        }

        // Print the character itself
        os << message[i];
    }
}

/* Actually prints the message to the given output stream. */
void Debugger::_log(std::ostream& os, Severity severity, const std::string& message, size_t extra_indent) {
    using namespace SeverityValues;

    // Acquire the lock for this function
    std::unique_lock<std::mutex> _log_lock(this->lock);

    // Determine the type of message to preprend to signify how it went
    switch(severity) {
        case auxillary:
            // Print as a prefix; i.e., a plain message with correct indents
            {
                // If this function is actually muted and we have a stack, do not display the message
                if (this->stack.size() > 0) {
                    for (size_t i = 0; i < this->muted.size(); i++) {
                        if (this->muted[i] == this->stack[this->stack.size() - 1].func_name) {
                            // Do not do anything
                            return;
                        }
                    }
                }

                // Otherwise, we're clear to print the message
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 7;
                os << std::string(this->indent_level * 7, ' ') << auxillary_msg;
                this->print_linewrapped(os, x, width, message);
                os << reset_msg << std::endl;
                return;
            }

        case info:
            // Print as info; pretty much the same, except that we now prepent with the info message
            {
                // If this function is actually muted and we have a stack, do not display the message
                if (this->stack.size() > 0) {
                    for (size_t i = 0; i < this->muted.size(); i++) {
                        if (this->muted[i] == this->stack[this->stack.size() - 1].func_name) {
                            // Do not do anything
                            return;
                        }
                    }
                }

                // Otherwise, we're clear to print the message
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 7;
                os << std::string(this->indent_level * 7, ' ') << info_msg;
                this->print_linewrapped(os, x, width, message);
                os << reset_msg << std::endl;
                return;
            }

        case warning:
            // Print as warning; the same as info with a different prexif, plus we print the origin of the line
            {
                // If this function is actually muted and we have a stack, do not display the message
                if (this->stack.size() > 0) {
                    for (size_t i = 0; i < this->muted.size(); i++) {
                        if (this->muted[i] == this->stack[this->stack.size() - 1].func_name) {
                            // Do not do anything
                            return;
                        }
                    }
                }

                // Print the new message as normal
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 7;
                os << std::string(this->indent_level * 7, ' ') << warning_msg;
                this->print_linewrapped(os, x, width, message);
                os << reset_msg << std::endl;

                // If there is a stack, display the stack message
                if (this->stack.size() > 0) {
                    Frame f = this->stack[this->stack.size() - 1];
                    std::string to_print = "[in function '\033[1m" + f.func_name + "\033[0m' at \033[1m" + f.file_name + ':' + std::to_string(f.line_number) + "\033[0m]";
                    os << std::string(7 + this->indent_level * 7, ' ');
                    x = 0;
                    this->print_linewrapped(os, x, width, to_print);
                    os << reset_msg << std::endl;
                }

                return;
            }

        case nonfatal:
            // Print as nonfatal error; add some extra spacing around the message, and provide the full stacktrace
            {
                size_t x = 0;
                size_t width = max_line_width - 7;
                os << nonfatal_msg;
                this->print_linewrapped(os, x, width, message);
                os << reset_msg << std::endl;

                // Print a stacktrace, if any
                if (this->stack.size() > 0) {
                    os << "       \033[1mStacktrace:\033[0m" << std::endl;
                    for (size_t i = 0; i < this->stack.size(); i++) {
                        x = 0;
                        Frame f = this->stack[this->stack.size() - 1 - i];
                        std::string prefix = i == 0 ? "in" : "from";
                        this->print_linewrapped(os, x, width, "       " + prefix + " function '\033[1m" + f.func_name + "\033[0m' at \033[1m" + f.file_name + ':' + std::to_string(f.line_number) + "\033[0m");
                        os << reset_msg << std::endl;
                    }
                    os << std::endl;
                }

                return;
            }

        case fatal:
            // Print as fatal error; add some extra spacing around the message, provide the full stacktrace, then throw the error
            {
                size_t x = 0;
                size_t width = max_line_width - 7;
                os << fatal_msg;
                this->print_linewrapped(os, x, width, message);
                os << reset_msg << std::endl;

                // Print a stacktrace, if any
                if (this->stack.size() > 0) {
                    os << "       \033[1mStacktrace:\033[0m" << std::endl;
                    for (size_t i = 0; i < this->stack.size(); i++) {
                        x = 0;
                        Frame f = this->stack[this->stack.size() - 1 - i];
                        std::string prefix = i == 0 ? "in" : "from";
                        this->print_linewrapped(os, x, width, "       " + prefix + " function '\033[1m" + f.func_name + "\033[0m' at \033[1m" + f.file_name + ':' + std::to_string(f.line_number) + "\033[0m");
                        os << reset_msg << std::endl;
                    }
                    os << std::endl;
                }

                // Return by throwing
                throw std::runtime_error(message);
            }

        default:
            // Let's re-do as auxillary
            this->_log(os, auxillary, message, extra_indent);
    }
}



/* Enters a new function, popping it's value on the stack. */
void Debugger::push(const std::string& function_name, const std::string& file_name, size_t line_number) {
    // Acquire the lock for this function
    std::unique_lock<std::mutex> push_lock(this->lock);

    // Create a new stack frame
    Frame frame({ function_name, file_name, line_number });

    // Push it on the stack
    this->stack.push_back(frame);
}

/* pops the top function name of the stack. */
void Debugger::pop() {
    // Acquire the lock for this function
    std::unique_lock<std::mutex> pop_lock(this->lock);

    // Only pop if there are elements left
    if (this->stack.size() > 0) {
        // Pop the vector
        this->stack.pop_back();
    }
}



/* Mutes a given function. All info-level severity messages that are called from it or from children functions are ignored. */
void Debugger::mute(const std::string& function_name) {
    // Acquire the lock for this function
    std::unique_lock<std::mutex> mute_lock(this->lock);

    // Add the given function to the vector of muted functions
    this->muted.push_back(function_name);
}

/* Unmutes a given function. All info-level severity messages that are called from it or from children functions are ignored. */
void Debugger::unmute(const std::string& function_name) {
    // Acquire the lock for this function
    std::unique_lock<std::mutex> unmute_lock(this->lock);

    // Try to find the function name
    for (std::vector<std::string>::iterator iter = this->muted.begin(); iter != this->muted.end(); ++iter) {
        if (*iter == function_name) {
            // Found it; remove it and we're done
            this->muted.erase(iter);
            return;
        }
    }

    // Not found, so nothing to remove
    return;
}



/* Increases indents. Useful for when a helper function is called, for example. */
void Debugger::indent() {
    // Acquire the lock for this function
    std::unique_lock<std::mutex> indent_lock(this->lock);

    // Simply add one to the value
    ++this->indent_level; 
}

/* Decreases indents. */
void Debugger::dedent() {
    // Acquire the lock for this function
    std::unique_lock<std::mutex> dedent_lock(this->lock);

    // Subtract one from the value, but make sure it's bounded to not go below zero and overflow
    this->indent_level = (this->indent_level > 0 ? this->indent_level - 1 : 0); 
}



/* Logs a message to the debugger. The type of message must be specified, which also determines how the message will be printed. If the the severity is fatal, also throws a std::runtime_error with the same text. To disable that, use Severity::nonfatal otherwise. Finally, one can optionally specify extra levels of indentation to use for this message. */
void Debugger::log(Severity severity, const std::string& message, size_t extra_indent) {
    // Write to the correct stream based on the severity
    if (severity == Severity::info || severity == Severity::auxillary) {
        this->_log(std::cout, severity, message, extra_indent);
    } else {
        this->_log(std::cerr, severity, message, extra_indent);
    }
}
