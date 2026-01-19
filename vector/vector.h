#ifndef VECTOR
#define VECTOR

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

typedef struct Vector{
    size_t size;
    size_t capacity;
    int element_size;
    void *data;
}Vector;

//------------------------------------------------------------------------------------------------------------------------------------\\
//here are some vector functions for working with simple data that doesn't point to any allocated memory.
//functions like Vector_copy and Vector_destroy will not work properly with data like strings, vectors, or any
//other data type that uses dynamically allocated memory.
//if you want to store data types that use dynamic memory, use the CVector functions.

//create a vector with enough sapce for 10 elements
Vector Vector_create(int element_size);
//create a vector with a desired amount of elemnts
Vector Vector_createCapacity(size_t capacity, int element_size);
//create a vector with a desired amount of elements and set it to be full
Vector Vector_createSize(size_t size, int element_size);
//create a vector with a desired amount of elements and fill it up with a value
Vector Vector_createSizeSet(size_t size, void *value, int element_size);
//create a vector with a desired amount of elements and fill it up with a value
Vector Vector_createCapacitySizeSet(size_t capacity, size_t size, void *value, int element_size);
//free the vector. If the elements also use dynamic memory, they don't get freed -> memory leaks
void Vector_destroy(Vector *vector);


//add an element to the end of a vector
void Vector_push(Vector *vector, const void *value);
//get the last element of a vector and return it in the "ret" variable. Removes the last element
void Vector_pop(Vector *vector, void *ret);
//insert an element anywhere inside a vecter, if inserting outside the used memory 
//(i.e. vector has 10 elements and we are inserting to position 15) nothing is done
void Vector_insert(Vector *vector, const size_t index, const void *value);
//rewrite one of the elements with a value
void Vector_set(Vector *vector, const size_t index, const void *value);
//copy the element at an index to the "ret" variable
void Vector_get(Vector *vector, const size_t index, void *ret);
//return the pointer to an element
void *Vector_getPointer(Vector *vector, const size_t index);
//set size of the vector to 0
void Vector_clear(Vector *vector);
//set capacity of the vector to the desired amount.
//If the new capacity is less than the current size the trailing elements will be lost
void Vector_resize(Vector *vector, size_t capacity);


//creates a copy of an existing vector. Returns the copy
Vector Vector_copyReturn(Vector *src);
//copies the vector "src" into the vector "dest". If vector dest is not empty, it frees the memory.
//if vector "dest" contains dynamic elements that have been alocated, they don't get freed -> memory leaks
void Vector_copy(Vector *dest, Vector *src);


//get the capacity of a vector
size_t Vector_getCapacity(Vector *vector);
//get the size (number of elements) of a vector
size_t Vector_getSize(Vector *vector);
//get the size of vector elements
int Vector_getSizeOfElements(Vector *vector);

//shifts all elements of a vector from the position "index" to the right 
void Vector_shiftRightFrom(Vector *vector, const size_t index);
//shifts all elements of a vector from the position "index" to the left 
void Vector_shiftLeftFrom(Vector *vector, const size_t index);
//shifts all elements of a vector from the position "index" to the right by "amount"
void Vector_shiftRightFromBy(Vector *vector, const size_t index, size_t amount);
//shifts all elements of a vector from the position "index" to the left by "amount"
void Vector_shiftLeftFromBy(Vector *vector, const size_t index, size_t amount);


//sort the vector according to the supplied compare function
void Vector_sort(const Vector *vector, const int (*Vector_compare)(const void *a, const void *b));


//this is the end of simple vector functions
//------------------------------------------------------------------------------------------------------------------------------------\\

//------------------------------------------------------------------------------------------------------------------------------------\\
//here are Vector functions that can properly handle dynamic data types 
//but require the vector to have pointers to copy and destroy functions
//by default, CVector behaves as a vector of simple data types (int, char, ...)


typedef void (*CVdestroy_func)(void *element);
typedef void (*CVcopy_func)(void *dst, const void *src);

typedef struct CVector{
    size_t size;
    size_t capacity;
    int element_size;
    void *data;

    CVdestroy_func destroy;
    CVcopy_func copy;
}CVector;

//create a vector with enough sapce for 10 elements
CVector CVector_create(int element_size, CVdestroy_func destroy, CVcopy_func copy);
//create a default 1D vector (Same as the simple Vector) with enough sapce for 10 elements
CVector CVector_createDefault1D(int element_size);
//create a default 2D vector (Vector of vectors) with enough sapce for 10 elements
CVector CVector_createDefault2D(int element_size);
//create a vector with a desired amount of elemnts
CVector CVector_createCapacity(size_t capacity, int element_size, CVdestroy_func destroy, CVcopy_func copy);
//create a vector with a desired amount of elements and set it to be full
CVector CVector_createSize(size_t size, int element_size, CVdestroy_func destroy, CVcopy_func copy);
//create a vector with a desired amount of elements and fill it up with a value
CVector CVector_createSizeSet(size_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy);
//create a vector with a desired amount of elements and fill it up with a value
CVector CVector_createCapacitySizeSet(size_t capacity, size_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy);
//free the vector. If the elements also use dynamic memory, they don't get freed -> memory leaks
void CVector_destroy(CVector *vector);
//wrapper that calls Vector_destroy under the hood
void Vector_destroy_wrapper(void *element);

//add an element to the end of a vector
void CVector_push(CVector *vector, const void *value);
//get the last element of a vector and return it in the "ret" variable. Removes the last element
void CVector_pop(CVector *vector, void *ret);
//insert an element anywhere inside a vecter, if inserting outside the used memory 
//(i.e. vector has 10 elements and we are inserting to position 15) nothing is done
void CVector_insert(CVector *vector, const size_t index, const void *value);
//rewrite one of the elements with a value
void CVector_set(CVector *vector, const size_t index, const void *value);
//copy the element at an index to the "ret" variable
void CVector_get(CVector *vector, const size_t index, void *ret);
//return the pointer to an element
void *CVector_getPointer(CVector *vector, const size_t index);
//set size of the vector to 0
void CVector_clear(CVector *vector);
//set capacity of the vector to the desired amount.
//If the new capacity is less than the current size the trailing elements will be lost
void CVector_resize(CVector *vector, size_t capacity);


//creates a copy of an existing vector. Returns the copy
CVector CVector_copyReturn(CVector *src);
//copies the vector "src" into the vector "dest". If vector dest is not empty, it frees the memory.
//if vector "dest" contains dynamic elements that have been alocated, they don't get freed -> memory leaks
void CVector_copy(CVector *dest, CVector *src);
//wrapper that calls Vector_copy under the hood
void Vector_copy_wrapper(void *dst, const void *src);

//get the capacity of a vector
size_t CVector_getCapacity(CVector *vector);
//get the size (number of elements) of a vector
size_t CVector_getSize(CVector *vector);
//get the size of vector elements
int CVector_getSizeOfElements(CVector *vector);

//shifts all elements of a vector from the position "index" to the right 
void CVector_shiftRightFrom(CVector *vector, const size_t index);
//shifts all elements of a vector from the position "index" to the left 
void CVector_shiftLeftFrom(CVector *vector, const size_t index);
//shifts all elements of a vector from the position "index" to the right by "amount"
void CVector_shiftRightFromBy(CVector *vector, const size_t index, size_t amount);
//shifts all elements of a vector from the position "index" to the left by "amount"
void CVector_shiftLeftFromBy(CVector *vector, const size_t index, size_t amount);


//sort the vector according to the supplied compare function
void CVector_sort(const CVector *vector, const int (*CVector_compare)(const void *a, const void *b));


#endif