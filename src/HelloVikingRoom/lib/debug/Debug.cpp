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
#include <chrono>
#include <ctime>
#include <sstream>
#include <stdexcept>

#include "Debug.hpp"

using namespace Debug;


/***** DEBUGGER CLASS *****/
/* Default constructor for the Debugger class. */
Debugger::Debugger() {}



/* Actually prints the message to the given output stream. */
void Debugger::_log(std::ostream& os, Severity severity, const std::string& message, size_t extra_indent) {
    // // Fetch the individual timestamp values as strings of the right size
    // if (severity != Severity::auxillary) {
    //     std::time_t cnow = std::time(0);
    //     std::tm* now = std::localtime(&cnow);
    //     std::string hours = std::to_string(now->tm_hour);
    //     if (hours.size() < 2) { hours = '0' + hours; }
    //     std::string minutes = std::to_string(now->tm_min);
    //     if (minutes.size() < 2) { minutes = '0' + minutes; }
    //     std::string seconds = std::to_string(now->tm_sec);
    //     if (seconds.size() < 2) { seconds = '0' + seconds; }
    //     std::string milliseconds = std::to_string((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) % 1000).count());
    //     while (milliseconds.size() < 3) { milliseconds = '0' + milliseconds; }

    //     // Already write to the output stream
    //     os << '[' << hours << ':' << minutes << ':' << seconds << '.' << milliseconds << "] ";
    // } else {
    //     os << "               ";
    // }

    // Determine the type of message to preprend to signify how it went
    std::string indicator;
    switch(severity) {
        case Severity::auxillary:
            os << auxillary_msg;
            break;

        case Severity::info:
            os << info_msg;
            break;

        case Severity::warning:
            os << warning_msg;
            break;

        case Severity::nonfatal:
            os << nonfatal_msg;
            break;

        case Severity::fatal:
            os << fatal_msg;
            break;

        default:
            os << "[" RED "????" TEXT "] ";
    }

    // Add indents
    os << std::string(this->indent_level * 3, ' ');

    // Write the message to the target stream, linewrapped
    size_t x = 0;
    size_t width = max_line_width - 7 - this->indent_level * 3;
    for (size_t i = 0; i < message.size(); i++) {
        if (++x >= width) {
            // Write a newline
            os << std::endl << std::string(7 + this->indent_level * 3, ' ');
            x = 0;
        }
        os << message[i];
    }

    // Done, always reset the colours
    os << TEXT << std::endl;

    // If fatal, throw the error
    if (severity == Severity::fatal) {
        throw std::runtime_error(message);
    }
}



/* Enters a new function, popping it's value on the stack. */
void Debugger::push(const std::string& function_name, const std::string& file_name) {
    // Create a new stack frame
    Frame frame({ function_name, file_name });

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
