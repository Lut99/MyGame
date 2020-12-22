/* COMMON.hpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:41:16 PM
 * Last edited:
 *   12/22/2020, 5:47:47 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File with common stuff for all the testfiles.
**/

#ifndef COMMON_HPP
#define COMMON_HPP

/***** USEFUL DEFINES *****/
/* Prints the intro for a whole new test run. */
#define TESTRUN(NAME) \
    cout << endl << "TEST RUN for " NAME << endl;
/* Prints the outtro for a whole new test run. */
#define ENDRUN(SUCCESS) \
    cout << "Run: " << ((SUCCESS) ? "\033[32;1mSUCCESS\033[0m" : "\033[31;1mFAIL\033[0m") << endl << endl; \
    return (SUCCESS);
/* Prints the intro for the given test case. */
#define TESTCASE(NAME) \
    cout << " > Testing " NAME "..." << flush;
/* Prints a failure message. */
#define ERROR(MESSAGE) \
    cout << endl << "   \033[31;1mERROR\033[0m: " MESSAGE << endl;
/* Prints the outtro for the given test case. */
#define ENDCASE(SUCCESS) \
    cout << ((SUCCESS) ? " \033[32;1mOK\033[0m" : "   Testcase failed.") << endl;

#endif
