#ifndef RS_VECTOR_H
#define RS_VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#ifndef SSIZE_T
#define SSIZE_T
    #if defined (_WIN32) || defined(_WIN64)
        typedef long long int ssize_t;
    #endif
#endif

typedef struct RSVec{
    size_t size;
    size_t capacity;
    int element_size;
    void *data;
}RSVec;

//empty vector with everything set to 0
#define RSVECTOR_NULL() (RSVec){0, 0, 0, NULL}
//empty vector using data type
#define RSVECTOR_EMPTY(type) (RSVec){0, 0, sizeof(type), NULL}
//empty vector using data type size
#define _RSVECTOR_EMPTY(element_size) (RSVec){0, 0, element_size, NULL}

//------------------------------------------------------------------------------------------------------------------------------------\\
//here are some vector functions for working with simple data that doesn't point to any allocated memory.
//functions like RSvec_copy and RSvec_destroy will not work properly with data like strings, vectors, or any
//other data type that uses dynamically allocated memory.
//if you want to store data types that use dynamic memory, use the RSManagedVec functions.

//create a vector with enough space for 10 elements
RSVec RSvec_create(int element_size);
//create a vector with a desired amount of elements
RSVec RSvec_createCapacity(ssize_t capacity, int element_size);
//create a vector with a desired amount of elements and set it to be full
RSVec RSvec_createSize(ssize_t size, int element_size);
//create a vector with a desired amount of elements and fill it up with a value
RSVec RSvec_createSizeSet(ssize_t size, void *value, int element_size);
//create a vector with a desired amount of elements and fill it up with a value
RSVec RSvec_createCapacitySizeSet(ssize_t capacity, ssize_t size, void *value, int element_size);
//free the vector. If the elements also use dynamic memory, they don't get freed -> memory leaks
void RSvec_destroy(RSVec *vector);


//add an element to the end of a vector
void RSvec_push(RSVec *vector, const void *value);
//copy an array of elements to the end of the vector
void RSvec_pushArray(RSVec *vector, const void *value, ssize_t amount);
//get the last element of a vector and return it in the "ret" variable. Removes the last element
int RSvec_pop(RSVec *vector, void *ret);
//insert an element anywhere inside a vector, if inserting outside the used memory 
//(i.e. vector has 10 elements and we are inserting to position 15) nothing is done
void RSvec_insert(RSVec *vector, const ssize_t index, const void *value);
//rewrite one of the elements with a value
void RSvec_set(RSVec *vector, const ssize_t index, const void *value);
//copy the element at an index to the "ret" variable
int RSvec_get(RSVec *vector, const ssize_t index, void *ret);
//return the pointer to an element
void *RSvec_getPointer(RSVec *vector, const ssize_t index);
//set size of the vector to 0
void RSvec_clear(RSVec *vector);
//set capacity (NOT SIZE) of the vector to the desired amount.
//If the new capacity is less than the current size the trailing elements will be lost
void RSvec_resize(RSVec *vector, ssize_t capacity);


//creates a copy of an existing vector. Returns the copy
RSVec RSvec_copyReturn(RSVec *src);
//copies the vector "src" into the vector "dest". If vector dest is not empty, it gets over written.
//if vector "dest" contains dynamic elements that have been alocated, they don't get freed -> memory leaks.
//dest vector MUST be destroyed before being copied into, or MUST be a refference attained from RSvec_getReference()
void RSvec_copy(RSVec *dest, RSVec *src);


//get the capacity of a vector
size_t RSvec_getCapacity(RSVec *vector);
//get the size (number of elements) of a vector
size_t RSvec_getSize(RSVec *vector);
//get the size of vector elements
int RSvec_getSizeOfElements(RSVec *vector);

//shifts all elements of a vector from the position "index" to the right 
void RSvec_shiftRightFrom(RSVec *vector, const ssize_t index);
//shifts all elements of a vector from the position "index" to the left 
void RSvec_shiftLeftFrom(RSVec *vector, const ssize_t index);
//shifts all elements of a vector from the position "index" to the right by "amount"
void RSvec_shiftRightFromBy(RSVec *vector, const ssize_t index, ssize_t amount);
//shifts all elements of a vector from the position "index" to the left by "amount"
void RSvec_shiftLeftFromBy(RSVec *vector, const ssize_t index, ssize_t amount);


//sort the vector according to the supplied compare function
void RSvec_sort(const RSVec *vector, const int (*RSvec_compare)(const void *a, const void *b));


//this is the end of simple vector functions
//------------------------------------------------------------------------------------------------------------------------------------\\

//------------------------------------------------------------------------------------------------------------------------------------\\
//here are RSVec functions that can properly handle dynamic data types 
//but require the vector to have pointers to copy and destroy functions
//by default, RSManagedVec behaves as a vector of simple data types (int, char, ...)


typedef void (*rsMVecDestroyFunc)(void *element);
typedef void (*rsMVecCopyFunc)(void *dst, const void *src);

typedef struct RSManagedVec{
    size_t size;
    size_t capacity;
    int element_size;
    void *data;

    rsMVecDestroyFunc destroy;
    rsMVecCopyFunc copy;
}RSManagedVec;

//empty RSManagedVec with everything set to 0
#define CVECTOR_NULL() (RSManagedVec){0, 0, 0, NULL, NULL, NULL}
//empty RSManagedVec using data type
#define CVECTOR_EMPTY(type, destroy, copy) (RSManagedVec){0, 0, sizeof(type), NULL, destroy, copy}
//empty RSManagedVec using element size
#define _CVECTOR_EMPTY(element_size, destroy, copy) (RSManagedVec){0, 0, element_size, NULL, destroy, copy}

//create a vector with enough space for 10 elements
RSManagedVec RSmvec_create(int element_size, rsMVecDestroyFunc destroy, rsMVecCopyFunc copy);
//create a default 1D vector (Same as the simple RSVec) with enough space for 10 elements
RSManagedVec RSmvec_createDefault1D(int element_size);
//create a default 2D vector (RSVec of vectors) with enough space for 10 elements
RSManagedVec RSmvec_createDefault2D(int element_size);
//create a vector with a desired amount of elements
RSManagedVec RSmvec_createCapacity(ssize_t capacity, int element_size, rsMVecDestroyFunc destroy, rsMVecCopyFunc copy);
//create a vector with a desired amount of elements and set it to be full
RSManagedVec RSmvec_createSize(ssize_t size, int element_size, rsMVecDestroyFunc destroy, rsMVecCopyFunc copy);
//create a vector with a desired amount of elements and fill it up with a value
RSManagedVec RSmvec_createSizeSet(ssize_t size, void *value, int element_size, rsMVecDestroyFunc destroy, rsMVecCopyFunc copy);
//create a vector with a desired amount of elements and fill it up with a value
RSManagedVec RSmvec_createCapacitySizeSet(ssize_t capacity, ssize_t size, void *value, int element_size, rsMVecDestroyFunc destroy, rsMVecCopyFunc copy);
//free the vector. If the elements also use dynamic memory, they don't get freed -> memory leaks
void RSmvec_destroy(RSManagedVec *vector);
//wrapper that calls RSvec_destroy under the hood
void RSvec_destroy_wrapper(void *element);

//add an element to the end of a vector
void RSmvec_push(RSManagedVec *vector, const void *value);
//get the last element of a vector and return it in the "ret" variable. Removes the last element
int RSmvec_pop(RSManagedVec *vector, void *ret);
//insert an element anywhere inside a vector, if inserting outside the used memory 
//(i.e. vector has 10 elements and we are inserting to position 15) nothing is done
void RSmvec_insert(RSManagedVec *vector, const ssize_t index, const void *value);
//rewrite one of the elements with a value
void RSmvec_set(RSManagedVec *vector, const ssize_t index, const void *value);
//copy the element at an index to the "ret" variable. If the elements contain dynamic memory, ret variable has to be freed
int RSmvec_get(RSManagedVec *vector, const ssize_t index, void *ret);
//copy the element at an index to the "ret" variable. If the elements contain pointers, data pointed to by these pointers
//is shared with the vector element and changing it will change the vector element. If the pointers point to dynamic memory
//this memory must not be freed by the ret variable
int RSmvec_getReference(RSManagedVec *vector, const ssize_t index, void *ret);
//return the pointer to an element
void *RSmvec_getPointer(RSManagedVec *vector, const ssize_t index);
//set size of the vector to 0
void RSmvec_clear(RSManagedVec *vector);
//set capacity (NOT SIZE) of the vector to the desired amount.
//If the new capacity is less than the current size the trailing elements will be lost
void RSmvec_resize(RSManagedVec *vector, ssize_t capacity);


//creates a copy of an existing vector. Returns the copy
RSManagedVec RSmvec_copyReturn(RSManagedVec *src);
//copies the vector "src" into the vector "dest". If vector dest is not empty, it gets over written.
//if vector "dest" contains dynamic elements that have been alocated, they don't get freed -> memory leaks.
//dest vector MUST be destroyed before being copied into, or MUST be a refference attained from RSmvec_getReference()
void RSmvec_copy(RSManagedVec *dest, RSManagedVec *src);
//wrapper that calls RSvec_copy under the hood
void RSvec_copy_wrapper(void *dst, const void *src);

//get the capacity of a vector
size_t RSmvec_getCapacity(RSManagedVec *vector);
//get the size (number of elements) of a vector
size_t RSmvec_getSize(RSManagedVec *vector);
//get the size of vector elements
int RSmvec_getSizeOfElements(RSManagedVec *vector);

//shifts all elements of a vector from the position "index" to the right 
void RSmvec_shiftRightFrom(RSManagedVec *vector, const ssize_t index);
//shifts all elements of a vector from the position "index" to the left 
void RSmvec_shiftLeftFrom(RSManagedVec *vector, const ssize_t index);
//shifts all elements of a vector from the position "index" to the right by "amount"
void RSmvec_shiftRightFromBy(RSManagedVec *vector, const ssize_t index, ssize_t amount);
//shifts all elements of a vector from the position "index" to the left by "amount"
void RSmvec_shiftLeftFromBy(RSManagedVec *vector, const ssize_t index, ssize_t amount);


//sort the vector according to the supplied compare function
void RSmvec_sort(const RSManagedVec *vector, const int (*RSmvec_compare)(const void *a, const void *b));


#endif