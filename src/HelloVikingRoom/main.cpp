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

#include "Debug.hpp"

using namespace std;


/***** ENTRY POINT *****/
int main() {
    cout << endl << "##### HELLO VIKINGROOM #####" << endl << endl;
    
    Debug::debugger.log(Debug::Severity::info, "Test");
}
