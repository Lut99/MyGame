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

#include "Debug.hpp"

using namespace Debug;


/***** DEBUGGER CLASS *****/
/* Default constructor for the Debugger class. */
Debugger::Debugger() {}



/* Prints a given string over multiple lines, pasting n spaces in front of each one and linewrapping on the target width. Optionally, a starting x can be specified. */
void Debugger::print_linewrapped(std::ostream& os, size_t& x, size_t width, const std::string& message) {
    // Get the string to be pasted in front of every new line
    std::string prefix = std::string(7 + this->indent_level * 3, ' ');
    // Loop to print each character
    for (size_t i = 0; i < message.size(); i++) {
        if (++x >= width) {
            os << std::endl << prefix;
            x = 0;
        }
        os << message[i];
    }
}

/* Actually prints the message to the given output stream. */
void Debugger::_log(std::ostream& os, Severity severity, const std::string& message, size_t extra_indent) {
    using namespace SeverityValues;

    // Determine the type of message to preprend to signify how it went
    switch(severity) {
        case auxillary:
            // Print as a prefix; i.e., a plain message with correct indents
            {
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 3;
                os << auxillary_msg << std::string(this->indent_level * 3, ' ');
                this->print_linewrapped(os, x, width, message);
                os << TEXT << std::endl;
                return;
            }

        case info:
            // Print as info; pretty much the same, except that we now prepent with the info message
            {
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 3;
                os << info_msg << std::string(this->indent_level * 3, ' ');
                this->print_linewrapped(os, x, width, message);
                os << TEXT << std::endl;
                return;
            }

        case warning:
            // Print as warning; the same as info with a different prexif, plus we print the origin of the line
            {
                std::string to_print = message;
                if (this->stack.size() > 0) {
                    Frame f = this->stack[this->stack.size() - 1];
                    to_print += "    [in function '\033[1m" + f.func_name + "\033[0m' at \033[1m" + f.file_name + ':' + std::to_string(f.line_number) + "\033[0m]";
                }

                // Print the new message as normal
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 3;
                os << warning_msg << std::string(this->indent_level * 3, ' ');
                this->print_linewrapped(os, x, width, to_print);
                os << TEXT << std::endl;
                return;
            }

        case nonfatal:
            // Print as nonfatal error; add some extra spacing around the message, and provide the full stacktrace
            {
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 3;
                os << nonfatal_msg;
                this->print_linewrapped(os, x, width, message);
                os << TEXT << std::endl;

                // Print a stacktrace, if any
                if (this->stack.size() > 0) {
                    os << "       \033[1mStacktrace:\033[0m" << std::endl;
                    for (size_t i = 0; i < this->stack.size(); i++) {
                        x = 0;
                        Frame f = this->stack[this->stack.size() - 1 - i];
                        std::string prefix = i == 0 ? "in" : "from";
                        this->print_linewrapped(os, x, width, "       " + prefix + " function '\033[1m" + f.func_name + "\033[0m' at \033[1m" + f.file_name + ':' + std::to_string(f.line_number) + "\033[0m");
                        os << TEXT << std::endl;
                    }
                    os << std::endl;
                }

                return;
            }

        case fatal:
            // Print as fatal error; add some extra spacing around the message, provide the full stacktrace, then throw the error
            {
                size_t x = 0;
                size_t width = max_line_width - 7 - this->indent_level * 3;
                os << fatal_msg;
                this->print_linewrapped(os, x, width, message);
                os << TEXT << std::endl;

                // Print a stacktrace, if any
                if (this->stack.size() > 0) {
                    os << "       \033[1mStacktrace:\033[0m" << std::endl;
                    for (size_t i = 0; i < this->stack.size(); i++) {
                        x = 0;
                        Frame f = this->stack[this->stack.size() - 1 - i];
                        std::string prefix = i == 0 ? "in" : "from";
                        this->print_linewrapped(os, x, width, "       " + prefix + " function '\033[1m" + f.func_name + "\033[0m' at \033[1m" + f.file_name + ':' + std::to_string(f.line_number) + "\033[0m");
                        os << TEXT << std::endl;
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
    // Create a new stack frame
    Frame frame({ function_name, file_name, line_number });

    // Push it on the stack
    this->stack.push_back(frame);
}



/* Unmutes a given function. All info-level severity messages that are called from it or from children functions are ignored. */
void Debugger::unmute(const std::string& function_name) {
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



/* Logs a message to the debugger. The type of message must be specified, which also determines how the message will be printed. If the the severity is fatal, also throws a std::runtime_error with the same text. To disable that, use Severity::nonfatal otherwise. Finally, one can optionally specify extra levels of indentation to use for this message. */
void Debugger::log(Severity severity, const std::string& message, size_t extra_indent) {
    // Write to the correct stream based on the severity
    if (severity == Severity::info || severity == Severity::auxillary) {
        this->_log(std::cout, severity, message, extra_indent);
    } else {
        this->_log(std::cerr, severity, message, extra_indent);
    }
}
