/* TEST ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:06:03 PM
 * Last edited:
 *   12/22/2020, 5:20:42 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that tests the Array class.
**/

#define FULLCOMPILE
#include "Tools/Array.hpp"

using namespace std;
using namespace Tools;

extern void aux1(const Tools::Array<int>& a);
extern void aux2(const Tools::Array<int>& a);

int main() {
    Array<int> a;
    aux1(a);
    aux2(a);
}
