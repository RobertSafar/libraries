//maybe change the shift left functions so that they work even if the user
//tells them to shift the vector from an index greater or equal to the size
//of the vector. So that for example if we tell a vector with vector.size = 10
//to shift left from index 10 (or more), it will ignore the fact that there
//is no element at the given index, and will shift all of the existing elements
//to the left

// #ifndef VECTOR_C
// #define VECTOR_C
#include "vector.h"


Vector Vector_create(int element_size){
    Vector vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

Vector Vector_createCapacity(size_t capacity, int element_size){
    Vector vector;
    vector.size = 0;
    vector.capacity = capacity;
    vector.element_size = element_size;
    vector.data = malloc(element_size * capacity);
    return vector;
}

Vector Vector_createSize(size_t size, int element_size){
    Vector vector;
    vector.size = size;
    vector.capacity = size;
    vector.element_size = element_size;
    vector.data = malloc(element_size * size);
    memset(vector.data, 0, (size * element_size));
    return vector;
}

Vector Vector_createSizeSet(size_t size, void *value, int element_size){
    Vector vector;
    vector.size = size;
    vector.capacity = size;
    vector.element_size = element_size;
    vector.data = malloc(element_size * size);
    for(size_t i=0; i<size; i++){
        memcpy((char*)vector.data + (i * element_size), value, element_size);
    }
    return vector;
}

Vector Vector_createCapacitySizeSet(size_t capacity, size_t size, void *value, int element_size){    Vector vector;
    vector.size = size;
    vector.capacity = capacity >= size ? capacity : size;
    vector.element_size = element_size;
    vector.data = malloc(element_size * size);
    for(size_t i=0; i<size; i++){
        memcpy((char*)vector.data + (i * element_size), value, element_size);
    }
    return vector;
}

void Vector_destroy(Vector *vector){
    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    return;
}



void Vector_push(Vector *vector, const void *value){
    if(!vector || !vector->data) return;

    if(vector->size >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
    }
    memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size);
    vector->size++;
    return;
}

void Vector_pop(Vector *vector, void *ret){
    if(!vector || !vector->data) return;

    if(vector->size == 0){
        return;
    }
    vector->size--;
    if(ret == NULL) {
        return;
    }
    memcpy(ret, (char*)vector->data + (vector->size * vector->element_size), vector->element_size);
    return;
}

void Vector_insert(Vector *vector, const size_t index, const void *value){
    if(!vector || !vector->data) return;
    
    if(index > vector->size){
        return;
    }
    else if(index == vector->size){
        Vector_push(vector, value);
        return;
    }
    Vector_shiftRightFrom(vector, index);
    memcpy((char*)vector->data + (index * vector->element_size), value, vector->element_size);
    return;
}

void Vector_set(Vector *vector, const size_t index, const void *value){
    if(!vector || !vector->data) return;
    
    if(index >= vector->size || index < -1){
        return;
    }
    memcpy((char*)vector->data + (index * vector->element_size), value, vector->element_size);
    return;
}

void Vector_get(Vector *vector, const size_t index, void *ret){
    if(!vector || !vector->data) return;
    
    if(index >= vector->size){
        return;
    }
    memcpy(ret, (char*)vector->data + (index * vector->element_size), vector->element_size);
    return;
}

void *Vector_getPointer(Vector *vector, const size_t index){
    if(!vector || !vector->data) return;
    
    if(index >= vector->size){
        return NULL;
    }
    return (char*)vector->data + (index * vector->element_size);
}

void Vector_clear(Vector *vector){
    if(!vector || !vector->data) return;
    
    vector->size = 0;
    return;
}

void Vector_resize(Vector *vector, size_t capacity){
    if(!vector || capacity < 0) return;
    
    if(vector->capacity == capacity) return;
    else if(vector->size < capacity) vector->size = capacity;
    vector->capacity = capacity;
    vector->data = realloc(vector->data, capacity * vector->element_size);
    return;
}


Vector Vector_copyReturn(Vector *src){
    if(!src) return (Vector){0, 0, 0, NULL};

    Vector vector;
    vector.size = src->size;
    vector.capacity = src->capacity;
    vector.element_size = src->element_size;
    vector.data = malloc(src->capacity * src->element_size);
    memcpy(vector.data, src->data, src->size * src->element_size);
    return vector;
}

void Vector_copy(Vector *dest, Vector *src){
    if(!dest || !src) return;

    if(dest->data != NULL){
        free(dest->data);
    }
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->element_size = src->element_size;
    dest->data = malloc(src->capacity * src->element_size);
    memcpy(dest->data, src->data, src->size * src->element_size);
    return;
}



size_t Vector_getCapacity(Vector *vector){
    if(vector) return vector->capacity;
    return -1;
}

size_t Vector_getSize(Vector *vector){
    if(vector) return vector->size;
    return -1;
}

int Vector_getSizeOfElements(Vector *vector){
    if(vector) return vector->element_size;
    return -1;
}



void Vector_shiftRightFrom(Vector *vector, const size_t index){
    if(!vector) return;

    if(index >= vector->size || index < 0){
        return;
    }
    else if((vector->size + 1) >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (index) * vector->element_size;
    memmove((char*)src + vector->element_size, src, (vector->size - index) * vector->element_size);
    
    vector->size++;
    return;
}

void Vector_shiftLeftFrom(Vector *vector, const size_t index){
    if(!vector) return;
    
    if(index >= vector->size || index < 0){
        return;
    }

    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + vector->element_size, index * vector->element_size);
    
    if(index == (vector->size-1)) vector->size--;
    return;
}

void Vector_shiftRightFromBy(Vector *vector, const size_t index, size_t amount){
    if(!vector || amount <= 0) return;

    if(index >= vector->size || index < 0){
        return;
    }
    else if((vector->size + amount) >= vector->capacity){
        vector->capacity = (vector->size + amount) * 1.5;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (index) * vector->element_size;
    memmove((char*)src + amount * vector->element_size, src, (vector->size - index) * vector->element_size);
    
    vector->size += amount;
    return;
}

void Vector_shiftLeftFromBy(Vector *vector, const size_t index, size_t amount){
    if(!vector || amount <= 0) return;
    
    if(index >= vector->size || index < 0){
        return;
    }

    size_t copy_amount = index >= amount ? index - amount + 1 : 0;
    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + amount * vector->element_size, copy_amount * vector->element_size);
    
    if(index == (vector->size-1)) vector->size = vector->size > amount ? vector->size - amount : 0;
    return;
}



void Vector_sort(const Vector *vector, const int (*Vector_compare)(const void *a, const void *b)){
    qsort(vector->data, vector->size, vector->element_size, Vector_compare);
    return;
}



//------------------------------------------------------------------------------------------------------------------



CVector CVector_create(int element_size, CVdestroy_func destroy, CVcopy_func copy){
    CVector vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = destroy;
    vector.copy = copy;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

CVector CVector_createDefault1D(int element_size){
    CVector vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = NULL;
    vector.copy = NULL;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

CVector CVector_createDefault2D(int element_size){
    CVector vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

CVector CVector_createCapacity(size_t capacity, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    CVector vector;
    vector.size = 0;
    vector.capacity = capacity;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

CVector CVector_createSize(size_t size, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    CVector vector;
    vector.size = size;
    vector.capacity = size;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

CVector CVector_createSizeSet(size_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    CVector vector;
    vector.size = size;
    vector.capacity = size;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    for(size_t i=0; i<size; i++){
        memcpy((char*)vector.data + (i * element_size), value, element_size);
    }
    return vector;
}

CVector CVector_createCapacitySizeSet(size_t capacity, size_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    CVector vector;
    vector.size = size;
    vector.capacity = capacity >= size ? capacity : size;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    for(size_t i=0; i<size; i++){
        memcpy((char*)vector.data + (i * element_size), value, element_size);
    }
    return vector;
}

void CVector_destroy(CVector *vector){
    if(!vector) return;
    if(vector->destroy == NULL){
        free(vector->data);
        return;
    }

    for(size_t i = 0; i < vector->size; i++){
        vector->destroy((char*)vector->data + vector->element_size * i);
    }
    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    return;
}

void Vector_destroy_wrapper(void *element){
    Vector_destroy((Vector*)element);
    return;
}



void CVector_push(CVector *vector, const void *value){
    if(!vector || !vector->data) return;

    if(vector->size >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
    }
    if(vector->copy) vector->copy((char*)vector->data + (vector->size * vector->element_size), value);
    else memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size);
    vector->size++;
    return;
}

void CVector_pop(CVector *vector, void *ret){
    if(!vector || !vector->data) return;

    if(vector->size == 0){
        return;
    }
    vector->size--;
    if(ret == NULL) {
        //destroy the element that is getting popped but not assigned to a variable
        if(vector->destroy) vector->destroy((char*)vector->data + vector->size);
        return;
    }
    memcpy(ret, (char*)vector->data + (vector->size * vector->element_size), vector->element_size);
    return;
}


void CVector_insert(CVector *vector, const size_t index, const void *value){
    if(!vector || !vector->data) return;

    if(index > vector->size){
        return;
    }
    else if(index == vector->size){
        Vector_push(vector, value);
        return;
    }
    Vector_shiftRightFrom(vector, index);

    if(vector->copy) vector->copy((char*)vector->data + (index * vector->element_size), value);
    else memcpy((char*)vector->data + (index * vector->element_size), value, vector->element_size);
    
    return;
}

void CVector_set(CVector *vector, const size_t index, const void *value){
    if(!vector || !vector->data) return;

    if(index >= vector->size || index < -1){
        return;
    }

    if(vector->copy) vector->copy((char*)vector->data + (index * vector->element_size), value);
    else memcpy((char*)vector->data + (index * vector->element_size), value, vector->element_size);
    return;
}

void CVector_get(CVector *vector, const size_t index, void *ret){
    if(!vector || !vector->data) return;

    if(index >= vector->size){
        return;
    }
    if(vector->copy) vector->copy(ret, (char*)vector->data + (index * vector->element_size));
    else memcpy(ret, (char*)vector->data + (index * vector->element_size), vector->element_size);
    return;
}

void *CVector_getPointer(CVector *vector, const size_t index){
    if(!vector || !vector->data) return;

    if(index >= vector->size){
        return NULL;
    }
    return (char*)vector->data + (index * vector->element_size);
}

void CVector_clear(CVector *vector){
    if(!vector || !vector->data) return;
    
    if(!vector->destroy){
        vector->size = 0;
        return;
    } 
    for(size_t i = 0; i < vector->size; i++){
        vector->destroy((char*)vector->data + vector->element_size * i);
    }
    vector->size = 0;

    return;
}

void CVector_resize(CVector *vector, size_t capacity){
    if(!vector || capacity < 0) return;
    
    if(vector->capacity == capacity) return;
    else if(vector->size < capacity){
        if(vector->destroy){
            for(size_t i = capacity; i < vector->size; i++){
                vector->destroy((char*)vector->data + vector->element_size * i);
            }
        }    
        vector->size = capacity;
    }
    vector->capacity = capacity;
    vector->data = realloc(vector->data, capacity * vector->element_size);
    return;
}



CVector CVector_copyReturn(CVector *src){
    if(!src) return (CVector){0, 0, 0, NULL, NULL, NULL};

    CVector vector;
    vector.size = src->size;
    vector.capacity = src->capacity;
    vector.element_size = src->element_size;
    vector.copy = src->copy;
    vector.destroy = src->destroy;
    vector.data = malloc(src->capacity * src->element_size);

    if(!vector.copy) memcpy(vector.data, src->data, src->size * src->element_size);
    else{
        size_t offset = 0;
        for(size_t i = 0; i < vector.size; i++){
            vector.copy((char*)vector.data + offset, (char*)src->data + offset);
            offset += vector.element_size;
        }
    }
    return vector;
}


void CVector_copy(CVector *dest, CVector *src){
    if(!dest || !src) return;

    if(dest->data != NULL){
        free(dest->data);
    }
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->element_size = src->element_size;
    dest->copy = src->copy;
    dest->destroy = src->destroy;
    dest->data = malloc(src->capacity * src->element_size);

    if(!dest->copy) memcpy(dest->data, src->data, src->size * src->element_size);
    else{
        size_t offset = 0;
        int element_size = dest->element_size;
        for(size_t i = 0; i < dest->size; i++){
            dest->copy((char*)dest->data + offset, (char*)src->data + offset);
            offset += element_size;
        }
    }
    return;
}

void Vector_copy_wrapper(void *src, const void *dst){
    Vector_copy((Vector*)dst, (Vector*)src);
}



size_t CVector_getCapacity(CVector *vector){
    if(vector) return vector->capacity;
    return -1;
}

size_t CVector_getSize(CVector *vector){
    if(vector) return vector->size;
    return -1;
}

int CVector_getSizeOfElements(CVector *vector){
    if(vector) return vector->element_size;
    return -1;
}


void CVector_shiftRightFrom(CVector *vector, const size_t index){
    if(!vector) return;

    if(index >= vector->size || index < 0){
        return;
    }
    else if((vector->size + 1) >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (index) * vector->element_size;
    memmove((char*)src + vector->element_size, src, (vector->size - index) * vector->element_size);
    memset(src, 0, vector->element_size);

    vector->size++;
    return;
}

void CVector_shiftLeftFrom(CVector *vector, const size_t index){
    if(!vector) return;
    
    if(index >= vector->size || index < 0){
        return;
    }

    if(vector->destroy) vector->destroy(vector->data);
    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + vector->element_size, index * vector->element_size);
    memset((char*)src + index * vector->element_size, 0, vector->element_size);

    if(index == (vector->size-1)) vector->size--;
    return;
}

void CVector_shiftRightFromBy(CVector *vector, const size_t index, size_t amount){
    if(!vector || amount <= 0) return;

    if(index >= vector->size || index < 0){
        return;
    }
    else if((vector->size + amount) >= vector->capacity){
        vector->capacity = (vector->size + amount) * 1.5;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (index) * vector->element_size;
    memmove((char*)src + amount * vector->element_size, src, (vector->size - index) * vector->element_size);
    memset(src, 0, amount * vector->element_size);

    vector->size += amount;
    return;
}

void CVector_shiftLeftFromBy(CVector *vector, const size_t index, size_t amount){
    if(!vector || amount <= 0) return;
    
    if(index >= vector->size || index < 0){
        return;
    }

    size_t copy_amount = index >= amount ? index - amount + 1 : 0;
    size_t destroy_amount = index + 1 - copy_amount;
    if(vector->destroy){
        char *destroy_add = (char*)vector->data;
        for(size_t i = 0; i < destroy_amount; i++){
            vector->destroy(destroy_add);
            destroy_add += vector->element_size;
        }
    }
    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + amount * vector->element_size, copy_amount * vector->element_size);
    memset((char*)src + copy_amount * vector->element_size, 0, destroy_amount * vector->element_size);

    if(index == (vector->size-1)) vector->size = vector->size > amount ? vector->size - amount : 0;
    return;
}



void CVector_sort(const CVector *vector, const int (*CVector_compare)(const void *a, const void *b)){
    qsort(vector->data, vector->size, vector->element_size, CVector_compare);
    return;
}


// #endif