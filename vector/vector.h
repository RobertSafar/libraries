#ifndef VECTOR
#define VECTOR

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

typedef struct Vector{
    void *data;
    size_t size;
    size_t capacity;
    int element_size;
}Vector;


//create a vector with enough sapce for 10 elements
Vector Vector_create(int element_size);
//create a vector with a desired amount of elemnts
Vector Vector_createCapacity(size_t capacity, int element_size);
//create a vector with a desired amount of elements and set it to be full
Vector Vector_createSize(size_t size, int element_size);
//create a vector with a desired amount of elements and fill it up with a value
Vector Vector_createSizeSet(size_t size, void *value, int element_size);
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
//set size of the vector to 0
void Vector_clear(Vector *vector);


//creates a copy of an existing vector. Returns the copy
Vector Vector_copyReturn(Vector *src);
//copies the vector "src" into the vector "dest". If vector dest is not empty, it frees the memory.
//if vector "dest" contains dynamic elements that have been alocated, they don't get freed -> memory leaks
void Vector_copy(Vector *dest, Vector *src);


//get the capacity of a vector
long long int Vector_getCapacity(Vector *vector);
//get the size (number of elements) of a vector
long long int Vector_getSize(Vector *vector);
//get the size of vector elements
long long int Vector_getSizeOfElements(Vector *vector);

//shifts all elements of a vector from the position "index" to the right 
void Vector_shiftRightFrom(Vector *vector, const size_t index);
//shifts all elements of a vector from the position "index" to the left 
void Vector_shiftLeftFrom(Vector *vector, const size_t index);


//sort the vector according to the supplied compare function
void Vector_sort(const Vector *vector, const int (*Vector_compare)(const void *a, const void *b));

#endif