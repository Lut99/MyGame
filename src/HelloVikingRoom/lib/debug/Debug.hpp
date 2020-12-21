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

/***** COLOUR CONSTANTS *****/
/* Foreground colours */
#define RED "\033[31;1m"
#define YELLOW "\033[33m"
#define GREEN "\033[32;1m"
/* Swaps the foreground and background colours. */
#define REVERSED "\033[7m"
/* Reset colour */
#define TEXT "\033[0m"

#ifdef DEBUG
/***** MACROS WHEN DEBUGGING IS ENABLED *****/

/* Wraps any function call, registring it on the debugger's stacktrace. */
#define DENTER(FUNC_NAME) \
    Debug::debugger.push((FUNC_NAME), (__FILE__), (__LINE__) - 1);
/* Wraps the return statement, first popping the current value from the stack. */
#define DRETURN \
    Debug::debugger.pop(); \
    return

/* Mutes function with the given name. */
#define DMUTE(FUNC_NAME) \
    Debug::debugger.mute((FUNC_NAME));
/* Unmutes function with the given name. */
#define DUNMUTE(FUNC_NAME) \
    Debug::debugger.mute((FUNC_NAME));

/* Increase the indent of the logger by N steps. */
#define DINDENT \
    Debug::debugger.indent();
/* Decrease the indent of the logger by N steps. */
#define DDEDENT \
    Debug::debugger.dedent();

/* Logs using the debugger. */
#define DLOG(SEVERITY, MESSAGE) \
    Debug::debugger.log((SEVERITY), (MESSAGE));
/* Logs using the debugger with extra indent. */
#define DLOGi(SEVERITY, MESSAGE, INDENT) \
    Debug::debugger.log((SEVERITY), (MESSAGE), (INDENT));


/* Increases the indent in the debug levels. */

#else
/***** MACROS WHEN DEBUGGING IS DISABLED *****/

/* Wraps any function call, registring it on the debugger's stacktrace. */
#define DENTER(FUNC_NAME)
/* Wraps the return statement, first popping the current value from the stack. */
#define DRETURN \
    return

/* Mutes function with the given name. */
#define DMUTE(FUNC_NAME)
/* Unmutes function with the given name. */
#define DUNMUTE(FUNC_NAME)

/* Increase the indent of the logger by N steps. */
#define DINDENT
/* Decrease the indent of the logger by N steps. */
#define DDEDENT

/* Logs using the debugger. */
#define DLOG(SEVERITY, MESSAGE)
/* Logs using the debugger with extra indent. */
#define DLOGi(SEVERITY, MESSAGE, INDENT)

#endif


/***** DEBUG NAMESPACE *****/
namespace Debug {
    /* The maximum linewidth before the debugger breaks lines. */
    static const size_t max_line_width = 100;
    /* The string that will be appended before all auxillary messages. */
    static const std::string auxillary_msg = "       ";
    /* The string that will be appended before all info messages. */
    static const std::string info_msg = "[" GREEN " OK " TEXT "] ";
    /* The string that will be appended before all warning messages. */
    static const std::string warning_msg = "[" YELLOW "WARN" TEXT "] ";
    /* The string that will be appended before all error messages. */
    static const std::string nonfatal_msg = "[" RED "FAIL" TEXT "] ";
    /* The string that will be appended before all error messages. */
    static const std::string fatal_msg = "[" RED REVERSED "FAIL" TEXT "] ";



    /* Enum that defines the possible debug message types. */
    namespace SeverityValues {
        enum type {
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
    }
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



    /* The main debug class, which is used to keep track of where we are and whether or not prints are accepted etc. */
    class Debugger {
    private:
        /* The stack of frames we're currently in. */
        std::vector<Frame> stack;
        /* List of currently muted functions. */
        std::vector<std::string> muted;

        /* The current number of indents specified. */
        size_t indent_level;

        /* Prints a given string over multiple lines, pasting n spaces in front of each one and linewrapping on the target width. Optionally, a starting x can be specified. */
        void print_linewrapped(std::ostream& os, size_t& x, size_t width, const std::string& message);
        /* Actually prints the message to a given stream. */
        void _log(std::ostream& os, Severity severity, const std::string& message, size_t extra_indent);

    public:
        /* Default constructor for the Debugger class. */
        Debugger();
        
        /* Enters a new function, popping it's value on the stack. */
        void push(const std::string& function_name, const std::string& file_name, size_t line_number);
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
