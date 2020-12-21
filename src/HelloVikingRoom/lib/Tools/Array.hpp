/* ARRAY.hpp
 *   by Lut99
 *
 * Created:
 *   21/12/2020, 13:39:20
 * Last edited:
 *   21/12/2020, 13:39:20
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Datastructure that wraps a simple array, that can resize but isn't
 *   build for it. Meant to be a lightweight wrapper around C-arrays.
**/

#ifndef ARRAY_HPP
#define ARRAY_HPP

#include <cstring>
#include <type_traits>
#include <initializer_list>
#include <string>
#include <stdexcept>
#include <iostream>

/* Disable the 'class memory access' warnings. */
#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif

namespace Tools {
    /* Lightweight array class for move constructible types, that can be used to resize but isn't build for it. */
    template <class T>
    class Array {
    private:
        /* C-array of the object type T we wrap. */
        T* raw_data;
        /* Number of elements, i.e., size, of the array. */
        size_t length;
        /* Number of elements we reserved space for. */
        size_t max_length;

    public:
        /* Default constructor for the Array class, which optionally takes the initial size of the array. */
        Array(size_t initial_size = 0) :
            length(0),
            max_length(initial_size)
        {
            // Make sure the element is move constructible
            static_assert(std::is_copy_constructible<T>::value || std::is_move_constructible<T>::value, "Can only create Arrays for elements that are move or copy constructible.");

            // Allocate enough memory using C's alloc, to avoid calling constructors and stuff
            this->raw_data = (T*) malloc(sizeof(T) * this->max_length);
            if (this->raw_data == nullptr) { throw std::bad_alloc(); }
        }
        /* Constructor for the Array class, which takes an initializer list. Requires the given Type to be copy constructible. */
        Array(const std::initializer_list<T>& list) :
            length(list.size()),
            max_length(list.size())
        {
            // Make sure the element is move constructible
            static_assert(std::is_copy_constructible<T>::value || std::is_move_constructible<T>::value, "Can only create Arrays for elements that are move or copy constructible.");
            // Make sure that the class is copy constructible
            static_assert(std::is_copy_constructible<T>::value, "Only Arrays for copy-constructible classes can be intialized with an initializer_list.");

            // Allocate enough space
            this->raw_data = (T*) malloc(sizeof(T) * this->max_length);
            if (this->raw_data == nullptr) { throw std::bad_alloc(); }

            // Copy each element
            size_t i = 0;
            for (const T& elem : list) {
                new(this->raw_data + i++) T(elem);
            }
        }
        /* Constructor for the Array class, which takes a pointer to a list of its type and the length of that list. */
        Array(const T* list, size_t list_size) :
            length(list_size),
            max_length(list_size)
        {
            // Make sure that the class is copy constructible
            static_assert(std::is_copy_constructible<T>::value, "Can only copy from an array who's elements are copy-constructible.");

            // Start by alocating a new internal array
            this->raw_data = (T*) malloc(sizeof(T) * this->max_length);
            if (this->raw_data == nullptr) { throw std::bad_alloc(); }

            // Copy each of the elements over
            for (size_t i = 0; i < this->length; i++) {
                new(this->raw_data + i) T(list[i]);
            }
        }
        /* Copy constructor for the Array class, which is only enabled if the type has a copy constructor. */
        Array(const Array& other) :
            length(other.length),
            max_length(other.max_length)
        {
            // Make sure that the class is copy constructible
            static_assert(std::is_copy_constructible<T>::value, "Can only copy arrays who's elements are copy-constructible.");

            // Start by alocating a new internal array
            this->raw_data = (T*) malloc(sizeof(T) * this->max_length);
            if (this->raw_data == nullptr) { throw std::bad_alloc(); }

            // Copy each of the elements over
            for (size_t i = 0; i < this->length; i++) {
                new(this->raw_data + i) T(other.raw_data[i]);
            }
        }
        /* Move constructor for the Array class. */
        Array(Array&& other) :
            raw_data(other.raw_data),
            length(other.length),
            max_length(other.max_length)
        {
            // Set the other's array to a nullptr, to avoid deallocation
            other.raw_data = nullptr;
        }
        /* Destructor for the Array class. */
        ~Array() {
            if (this->raw_data != nullptr) {
                // Loop through all elements to delete them
                for (size_t i = 0; i < this->length; i++) {
                    this->raw_data[i].~T();
                }
                free(this->raw_data);
            }
        }

        /* Adds a new element to the array, copying the object if possible. */
        void push_back(const T& new_element) {
            // Make sure that the class is copy constructible
            static_assert(std::is_copy_constructible<T>::value, "Can only copy from elements that have a copy constructor.");

            // Make sure there's enough memory
            if (this->length >= this->max_length) {
                this->reserve(this->length + 1);
            }

            // Add at the back of the number of elements by calling its copy constructor
            new(this->raw_data + this->length++) T(new_element);
        }
        /* Adds a new element to the array, leaving the given object in an unuseable state. */
        void push_back(T&& new_element) {
            // Make sure that the class is move constructible
            static_assert(std::is_move_constructible<T>::value, "Can only steal from elements that have a move constructor.");

            // Make sure there's enough memory
            if (this->length >= this->max_length) {
                this->reserve(this->length + 1);
            }

            // Add at the back of the number of elements by calling its copy constructor
            new(this->raw_data + this->length++) T(std::move(new_element));
        }

        /* Returns a muteable, C-style pointer to the internal array. Be careful with this, as this might mess with the memory management of the array! */
        inline T* data() { return this->raw_data; }
        /* Returns a non-muteable, C-style pointer to the internal array. Be careful with this, as this might mess with the memory management of the array! */
        inline const T* data() const { return this->raw_data; }
        /* Provides muteable access to a single element in the array. */
        inline T& operator[](size_t index) { return this->raw_data[index]; }
        /* Provides constant access to a single element in the array. */
        inline const T& operator[](size_t index) const { return this->raw_data[index]; }

        /* Explicitly reserves enough space for N elements. If N is bigger than the array currently is, the new elements will be left unitialized, whereas if N is smaller than the current size, the rightmost elements will be discarded. */
        void reserve(size_t N) {
            if (this->max_length == N) {
                // Nothing to be done
                return;
            }

            // Allocate enough space for the new array
            T* new_data = (T*) malloc(sizeof(T) * N);
            if (new_data == nullptr) { throw std::bad_alloc(); }

            // Copy at most N elements over to the new data
            size_t n_to_copy = this->length >= N ? N : this->length;
            memmove(new_data, this->raw_data, sizeof(T) * n_to_copy);

            // If we have left elements out, deallocate those before deallocating the array
            for (size_t i = N; i < this->length; i++) {
                this->raw_data[i].~T();
            }
            free(this->raw_data);

            // Populate the class with new elements
            this->raw_data = new_data;
            this->length = this->length >= N ? N : this->length;
            this->max_length = N;
        }
        /* Explicitly reserves enough space for N elements, and calls the default constructor for new elements. Guarantees that the entire array will be initialized. */
        void resize(size_t N) {
            // Make sure that the class is default constructible
            static_assert(std::is_default_constructible<T>::value, "Can only resize Arrays for elements that have default constructors.");

            // First, resize to the new amount of elements
            this->reserve(N);

            // Next, populate the remaining elements with the default constructor
            for (size_t i = this->length; i < this->max_length; i++) {
                new(this->raw_data + i) T;
            }
            // Update the length
            this->length = this->max_length;
        }

        /* Returns the number of elements stored in the array. */
        inline size_t size() const { return this->length; }
        /* Returns the number of elements we have space for, not the number of elements explicitly set in this array. */
        inline size_t capacity() const { return this->max_length; }

        /* Erases an element from the array by index. */
        void erase(size_t index) {
            if (index >= this->length) {
                throw std::out_of_range("Index " + std::to_string(index) + " is out of range for an Array of size " + std::to_string(this->length));
            }

            // Deallocate the element at the given index
            this->raw_data[index].~T();

            // Move the entire block of memory one back
            memmove(this->raw_data + index, this->raw_data + index + 1, sizeof(T) * (this->length - index - 1));
        }

        /* Copy assignment operator for the Array class. Only enabled if the type has a copy constructor available. */
        inline Array& operator=(const Array& other) {
            // Make sure that the class is copy constructible
            static_assert(std::is_copy_constructible<T>::value, "Can only copy-assign Arrays for elements with a copy constructor.");

            // If so, return
            return *this = Array(other); 
        }
        /* Move assignment operator for the Array class. */
        Array& operator=(Array&& other) {
            if (this != &other) { swap(*this, other); }
            return *this;
        }
        /* Swap operator for the Array class. */
        friend void swap(Array& a1, Array& a2) {
            using std::swap;

            swap(a1.data, a2.data);
            swap(a1.length, a2.length);
            swap(a1.max_length, a2.max_length);
        }
    };

}

// Re-enable the warnings
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif

#endif
