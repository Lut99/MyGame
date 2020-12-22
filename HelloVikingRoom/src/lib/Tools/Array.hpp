/* ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:00:01 PM
 * Last edited:
 *   12/22/2020, 5:28:57 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Our own Array class, which is optimizing for largely remaining the
 *   same size, but with options to change if needed.
**/

#ifndef ARRAY_HPP
#define ARRAY_HPP

/***** HEADER *****/
#include <cstddef>
#include <vector>
#include <initializer_list>

namespace Tools {
    /* The Array class, which can be used as a container for many things. */
    template <class T>
    class Array {
    private:
        /* The internal data. */
        T* elements;
        /* The number of internal elements. */
        size_t length;
        /* The maximum number of elements we allocated for. */
        size_t max_length;

    public:
        /* Default constructor for the Array class, which initializes it to zero. */
        Array();
        /* Constructor for the Array class, which takes an initial amount to size to. Each element will thus be uninitialized. */
        Array(size_t initial_size);
        /* Constructor for the Array class, which takes an initializer_list to initialize the Array with. */
        Array(const std::initializer_list<T>& list);
        /* Constructor for the Array class, which takes a raw C-style vector to copy elements from and its size. Note that the Array's element type must have a copy custructor defined. */
        Array(T* list, size_t list_size);
        /* Constructor for the Array class, which takes a C++-style vector. Note that the Array's element type must have a copy custructor defined. */
        Array(const std::vector<T>& list);
        /* Copy constructor for the Array class. Note that this only works if the Array's element has a copy constructor defined. */
        Array(const Array& other);
        /* Move constructor for the Array class. */
        Array(Array&& other);
        /* Destructor for the Array class. */
        ~Array();
    };
}





/***** IMPLEMENTATIONS *****/
#ifdef FULLCOMPILE
#include "Tools/Array.cpp"
#endif

#endif
