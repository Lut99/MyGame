/* MAIN.cpp
 *   by Lut99
 *
 * Created:
 *   19/12/2020, 16:23:17
 * Last edited:
 *   19/12/2020, 16:23:17
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This is the more mature version of our implementation for the Vulkan
 *   Tutorial, where we (hopefully) make the code a lot more structured by
 *   subdividing things in classes in their own files. Additionally, we
 *   move past the triangle here, to a square and then to a sample
 *   3D-model, which is a Viking Room.
**/

#include <iostream>
#include <exception>

#include "Debug.hpp"

using namespace std;


/***** ENTRY POINT *****/
int main() {
    cout << endl << "##### HELLO VIKINGROOM #####" << endl << endl;
    
    Debug::debugger.log(Debug::Severity::auxillary, "Auxillary");
    Debug::debugger.log(Debug::Severity::info, "TeshhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhtTesthhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhht");
    Debug::debugger.log(Debug::Severity::warning, "Warning");
    Debug::debugger.log(Debug::Severity::nonfatal, "Non-Fatal error");
    try {
        Debug::debugger.log(Debug::Severity::fatal, "Fatal error");
    } catch (std::exception& e) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
