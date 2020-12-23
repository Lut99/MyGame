/* PUSH ERASE.cpp
 *   by Lut99
 *
 * Created:
 *   12/23/2020, 5:31:23 PM
 * Last edited:
 *   12/23/2020, 5:39:32 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Checks the push_back() and erase() functions of the Array class.
**/

#include <iostream>
#include <cstddef>
#include <cstring>

#define FULLCOMPILE
#include "Tools/Array.hpp"
#include "common.hpp"

using namespace std;
using namespace Tools;


/***** TESTS *****/
/* Function that tests if the Array's push_back() works for an easy case (array with pre-reserved size). */
bool test_reserved_push() {
    TESTCASE("reserved push")

    // Get us a new array with an initial size
    Array<hard_t> test(3);
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    if (test.size() != 3) {
        ERROR("Pushing to reserved array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (size_t i = 0; i < 3; i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to reserved array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}

/* Function that tests if the Array's reserve() work for a slightly harder case (non-reserved Array). */
bool test_nonreserved_push() {
    TESTCASE("non-reserved push")

    // Get us a new array with an initial size
    Array<hard_t> test;
    test.push_back(hard_t(1));
    test.push_back(hard_t(2));
    test.push_back(hard_t(3));
    if (test.size() != 3) {
        ERROR("Pushing to reserved array failed; incorrect size: expected " + std::to_string(3) + ", got " + std::to_string(test.size()));
        ENDCASE(false);
    }

    // Test if the values made it
    for (size_t i = 0; i < 3; i++) {
        if (*test[i] != i + 1) {
             ERROR("Pushing to reserved array failed; incorrect value at index " + std::to_string(i) + ": expected " + std::to_string(i + 1) + ", got " + std::to_string(*test[i]));
            ENDCASE(false);
        }
    }

    ENDCASE(true);
}





/***** ENTRY POINT *****/
bool test_push_erase() {
    TESTRUN("Array push_back(), pop_back() and erase()");

    if (!test_reserved_push()) {
        ENDRUN(false);
    }
    if (!test_nonreserved_push()) {
        ENDRUN(false);
    }

    ENDRUN(true);
}
