/* ARRAY.cpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 4:59:25 PM
 * Last edited:
 *   12/22/2020, 5:34:50 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Our own Array class, which is optimizing for largely remaining the
 *   same size, but with options to change if needed.
**/

#include <stdexcept>

#include "Tools/Array.hpp"

using namespace std;
using namespace Tools;


/***** ARRAY CLASS ****/
/* Default constructor for the Array class, which initializes it to zero. */
template <class T> Array<T>::Array() :
    elements(nullptr),
    length(0),
    max_length(0)
{}

/* Constructor for the Array class, which takes an initial amount to size to. Each element will thus be uninitialized. */
template <class T> Array<T>::Array(size_t initial_size) :
    length(initial_size),
    max_length(initial_size)
{
    // Allocate enough space
    this->elements = (T*) malloc(sizeof(T) * this->max_length);
    if (this->elements == nullptr) { throw std::bad_alloc(); }
}

/* Constructor for the Array class, which takes an initializer_list to initialize the Array with. */
template <class T> Array<T>::Array(const std::initializer_list<T>& list) :
    Array(list.size())
{
    // Copy all the elements over
    size_t i = 0;
    for (const T& elem : list) {
        // Use the placement new to call T's copy constructor
        new(this->elements + i) T(elem);
        i++;
    }
}

/* Constructor for the Array class, which takes a raw C-style vector to copy elements from and its size. Note that the Array's element type must have a copy custructor defined. */
template <class T> Array<T>::Array(T* list, size_t list_size) :
    Array(list_size)
{
    // Copy all the elements over
    for (size_t i = 0; i < list_size; i++) {
        // Use the placement new to call T's copy constructor
        new(this->elements + i) T(list[i]);
    }
}

/* Constructor for the Array class, which takes a C++-style vector. Note that the Array's element type must have a copy custructor defined. */
template <class T> Array<T>::Array(const std::vector<T>& list) :
    Array(list.size())
{
    // Copy all the elements over
    for (size_t i = 0; i < list.size(); i++) {
        // Use the placement new to call T's copy constructor
        new(this->elements + i) T(list[i]);
    }
}

/* Copy constructor for the Array class. Note that this only works if the Array's element has a copy constructor defined. */
template <class T> Array<T>::Array(const Array& other) :
    length(other.length),
    max_length(other.max_length)
{
    // Allocate a new list of T's
    this->elements = (T*) malloc(sizeof(T) * other.max_length);
    if (this->elements == nullptr) { throw std::bad_alloc(); }

    // Copy each element over
    for (size_t i = 0; i < other.length; i++) {
        new(this->elements + i) T(other.elements[i]);
    }
}

/* Move constructor for the Array class. */
template <class T> Array<T>::Array(Array&& other) :
    elements(other.elements),
    length(other.length),
    max_length(other.max_length)
{
    // Tell the other one that it's over
    other.elements = nullptr;
}

/* Destructor for the Array class. */
template <class T> Array<T>::~Array() {
    // Only deallocate everything if not a nullptr
    if (this->elements != nullptr) {
        // First deallocate all elements
        for (size_t i = 0; i < this->length; i++) {
            this->elements[i].~T();
        }
        // Now, free the list itself
        free(this->elements);
    }
}
