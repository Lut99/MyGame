/* ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   12/22/2020, 5:00:01 PM
 * Last edited:
 *   12/23/2020, 5:31:56 PM
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
#include <limits>

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

        /* Adds a new element of type T to the array, copying it. Note that this requires the element to be copy constructible. */
        void push_back(const T& elem);
        /* Adds a new element of type T to the array, leaving it in an usused state (moving it). Note that this requires the element to be move constructible. */
        void push_back(T&& elem);
        /* Removes the last element from the array. */
        void pop_back();
        
        /* Erases an element with the given index from the array. Does nothing if the index is out-of-bounds. */
        void erase(size_t index);
        /* Erases multiple elements in the given (inclusive) range from the array. Does nothing if the any index is out-of-bounds or if the start_index is larger than the stop_index. */
        void erase(size_t start_index, size_t stop_index);
        /* Erases everything from the array, even removing the internal allocated array. */
        void clear();

        /* Re-allocates the internal array to the given size. Any leftover elements will be left unitialized, and elements that won't fit will be deallocated. */
        void reserve(size_t new_size);
        /* Resizes the array to the given size. Any leftover elements will be initialized with their default constructor, and elements that won't fit will be deallocated. */
        void resize(size_t new_size);

        /* Returns a muteable reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline T& operator[](size_t index) { return this->elements[index]; }
        /* Returns a constant reference to the element at the given index. Does not perform any in-of-bounds checking. */
        inline const T& operator[](size_t index) const { return this->elements[index]; }
        /* Returns a muteable reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        T& at(size_t index);
        /* Returns a constant reference to the element at the given index. Performs in-of-bounds checks before accessing the element. */
        const T& at(size_t index) const;

        /* Returns a muteable pointer to the internal data struct. Use this to fill the array using C-libraries, but beware that the array needs to have enough space reserved. Also note that object put here will still be deallocated by the Array using ~T(). The optional new_size parameter is used to update the size() value of the array, so it knows what is initialized and what is not. Leave it at numeric_limits<size_t>::max() to leave the array size unchanged. */
        T* const wdata(size_t new_size = std::numeric_limits<size_t>::max());
        /* Returns a constant pointer to the internal data struct. Use this to read from the array using C-libraries, but beware that the array needs to have enough space reserved. */
        inline const T* const rdata() const { return this->elements; }
        /* Returns true if there are no elements in this array, or false otherwise. */
        inline bool empty() const { return this->length == 0; }
        /* Returns the number of elements stored in this Array. */
        inline size_t size() const { return this->length; }
        /* Returns the number of elements this Array can store before resizing. */
        inline size_t capacity() const { return this->max_length; }

        /* Copy assignment operator for the Array class. Depends on Array's copy constructor, and therefore requires the Array's type to be copy constructible. */
        inline Array<T>& operator=(const Array<T>& other) { return *this = Array(other); }
        /* Move assignment operator for the Array class. */
        Array<T>& operator=(Array<T>&& other);
        /* Swap operator for the Array class. */
        friend void swap(Array& a1, Array& a2) {
            using std::swap;

            swap(a1.elements, a2.elements);
            swap(a1.length, a2.length);
            swap(a1.max_length, a2.max_length);
        }
    };
}





/***** IMPLEMENTATIONS *****/
#include "Tools/Array.cpp"

#endif
