/* INITIALIZATION.cpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:37:41 PM
 * Last edited:
 *   12/22/2020, 5:59:02 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   File that tests the intitializations part of the Array.
**/

#include <iostream>

#include "Tools/Array.hpp"
#include "common.hpp"

using namespace std;
using namespace Tools;


/***** HELPER CLASSES *****/
/* The easiest class; just stores a value. */
template <class T>
class ValueClass {
private:
    /* The value it stores. */
    T value;

public:
    /* Constructor for the ValueClass, which takes the value it should store. */
    ValueClass(const T& value) : value(value) {}
    
    /* Returns a muteable reference to the internal value class. */
    inline T& operator*() { return this->value; }
    /* Returns a constant reference to the internal value class. */
    inline const T& operator*() const { return this->value; }

};

/* A bit harder class: stores a value on the heap. */
template <class T>
class HeapClass {
private:
    /* The value it stores. */
    T* value;

public:
    /* Constructor for the HeapClass, which takes the value it should store. */
    HeapClass(const T& value) : value(new T(value)) {}
    /* Copy constructor for the HeapClass. */
    HeapClass(const HeapClass& other) : value(new T(*other.value)) {}
    /* Move constructor for the HeapClass. */
    HeapClass(HeapClass&& other) : value(other.value) {
        other.value = nullptr;
    }
    /* Destructor for the HeapClass. */
    ~HeapClass() {
        if (this->value != nullptr) { delete value; }
    }
    
    /* Returns a muteable reference to the internal value class. */
    inline T& operator*() { return *this->value; }
    /* Returns a constant reference to the internal value class. */
    inline const T& operator*() const { return *this->value; }

    /* Copy assignment operator for the HeapClass. */
    inline HeapClass& operator=(const HeapClass& other) { return *this = HeapClass(other); }
    /* Move assignment operator for the HeapClass. */
    HeapClass& operator=(HeapClass&& other) {
        using std::swap;
        if (this != &other) { swap(this->value, other.value); }
        return *this;
    }
};





/***** USING *****/
using easy = int;
using medium = ValueClass<easy>;
using medium_hard = ValueClass<medium>;
using hard = HeapClass<int>;
using super_hard = HeapClass<hard>;
using extreme = PolymorphicClass;





/***** TESTS *****/
/* First test, that checks if the default constructor works. */
bool test_default_constructor() {
    TESTCASE("default constructor");

    // Try to allocate the Array with a very easy type
    Array<easy> test1;
    // Next, try a harder type
    Array<medium> test2;

    ENDCASE(true);
}





/***** ENTRY POINT *****/
bool test_initialization() {
    TESTRUN("initialization / destruction");
    ENDRUN(true);
}
