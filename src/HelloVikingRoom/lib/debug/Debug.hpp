/* DEBUG.hpp
 *   by Lut99
 *
 * Created:
 *   19/12/2020, 16:32:58
 * Last edited:
 *   19/12/2020, 16:32:58
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

#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <string>
#include <vector>

#ifdef DEBUG
/***** MACROS WHEN DEBUGGING IS ENABLED *****/

/* Use this when entering a function, to log it's usage on the debugging stack. */
#define DEBUG_ENTER(FUNC_NAME) \
    Debug::debugger.push_function((FUNC_NAME), (__FILE__));

/* Use this to return from a function with a value, popping the function from the debugging stack. */
#define DEBUG_RETURN(VALUE) \
    Debug::debugger.pop(); \
    return (VALUE);
/* Use this to return from a function without a value, popping the function from the debugging stack. */
#define DEBUG_RETURN_VOID \
    Debug::debugger.pop(); \
    return;

#else
/***** MACROS WHEN DEBUGGING IS DISABLED *****/

/* Use this when entering a function, to log it's usage on the debugging stack. */
#define DEBUG_PUSH(FUNC_NAME)
/* Use this to return from a function with a value, popping the function from the debugging stack. */
#define DEBUG_RETURN(VALUE) \
    return (VALUE);
/* Use this to return from a function without a value, popping the function from the debugging stack. */
#define DEBUG_RETURN_VOID \
    return;

#endif


/***** DEBUG NAMESPACE *****/
namespace Debug {
    /* The maximum linewidth before the debugger breaks lines. */
    static const size_t max_line_width = 100;
    /* The string that will be appended before all auxillary messages. */
    static const std::string auxillary_msg = "       ";
    /* The string that will be appended before all info messages. */
    static const std::string info_msg = "[\033[32;1m OK \033[0m] ";
    /* The string that will be appended before all warning messages. */
    static const std::string warning_msg = "[\033[33;1mWARN\033[0m] ";
    /* The string that will be appended before all error messages. */
    static const std::string error_msg = "[\033[31;1mFAIL\033[0m] ";



    /* Enum that defines the possible debug message types. */
    enum class Severity {
        // Only provides the necessary indents, but does not print a message.
        auxillary,
        // Prints a message with 'OK' prepended to it
        info,
        // Prints a message with 'WARN' prepended to it
        warning,
        // Prints a message with 'FAIL' prepended to it
        nonfatal,
        // Prints a message with 'FAIL' prepended to it, then throws a std::runtime_error
        fatal
    };

    /* Struct used to refer to a stack frame. */
    struct Frame {
        /* The name of the function we entered. */
        std::string func_name;
        /* The file where the function resides. */
        std::string file_name;
    };



    /* The main debug class, which is used to keep track of where we are and whether or not prints are accepted etc. */
    class Debugger {
    private:
        /* The stack of frames we're currently in. */
        std::vector<Frame> stack;
        /* List of currently muted functions. */
        std::vector<std::string> muted;

        /* The current number of indents specified. */
        size_t indent_level;

        /* Actually prints the message to a given stream. */
        void _log(std::ostream& os, Severity severity, const std::string& message, size_t extra_indent);

    public:
        /* Default constructor for the Debugger class. */
        Debugger();
        
        /* Enters a new function, popping it's value on the stack. */
        void push(const std::string& function_name, const std::string& file_name);
        /* pops the top function name of the stack. */
        inline void pop() { this->stack.pop_back(); }

        /* Mutes a given function. All info-level severity messages that are called from it or from children functions are ignored. */
        inline void mute(const std::string& function_name) { this->muted.push_back(function_name); }
        /* Unmutes a given function. All info-level severity messages that are called from it or from children functions are ignored. */
        void unmute(const std::string& function_name);

        /* Increases indents. Useful for when a helper function is called, for example. */
        inline void indent() { ++this->indent_level; }
        /* Decreases indents. */
        inline void dedent() { this->indent_level = (this->indent_level > 0 ? this->indent_level - 1 : 0); }

        /* Logs a message to stdout. The type of message must be specified, which also determines how the message will be printed. If the the severity is fatal, also throws a std::runtime_error with the same text. To disable that, use Severity::nonfatal otherwise. Finally, one can optionally specify extra levels of indentation to use for this message. */
        void log(Severity severity, const std::string& message, size_t extra_indent = 0);

    };


    
    /* Static instance of the debug class, used for debugging. */
    static Debugger debugger;
};

#endif
