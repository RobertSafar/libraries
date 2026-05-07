//maybe change the shift left functions so that they work even if the user
//tells them to shift the vector from an index greater or equal to the size
//of the vector. So that for example if we tell a vector with vector.size = 10
//to shift left from index 10 (or more), it will ignore the fact that there
//is no element at the given index, and will shift all of the existing elements
//to the left

// #ifndef VECTOR_C
// #define VECTOR_C
#include "vector.h"

#ifdef _WIN32
    #ifdef BUILDING_DLL
        #define VAPI __declspec(dllexport)
    #elif defined(USING_DLL)
        #define VAPI __declspec(dllimport)
    #else
        #define VAPI
    #endif
#else
  #define VAPI   /* nothing */
#endif



VAPI Vector Vector_create(int element_size){
    Vector vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

VAPI Vector Vector_createCapacity(ssize_t capacity, int element_size){
    if (capacity < 0) return _VECTOR_EMPTY(element_size);

    size_t cap = (size_t) capacity;
    Vector vector;
    vector.size = 0;
    vector.capacity = cap;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    return vector;
}

VAPI Vector Vector_createSize(ssize_t size, int element_size){
    if (size < 0) return _VECTOR_EMPTY(element_size);

    size_t sz = (size_t) size;
    Vector vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    memset(vector.data, 0, (vector.size * vector.element_size));
    return vector;
}

VAPI Vector Vector_createSizeSet(ssize_t size, void *value, int element_size){
    if (size < 0) return _VECTOR_EMPTY(element_size);

    size_t sz = (size_t) size;
    Vector vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * element_size), value, element_size);
    }
    return vector;
}

VAPI Vector Vector_createCapacitySizeSet(ssize_t capacity, ssize_t size, void *value, int element_size){    
    if (capacity < 0) return _VECTOR_EMPTY(element_size);
    else if (capacity < size) size = capacity;
    else if (size < 0) size = 0;
    
    size_t cap = (size_t) capacity;
    size_t sz = (size_t) size;
    
    Vector vector;
    vector.size = sz;
    vector.capacity = cap >= sz ? cap : sz;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * vector.element_size), value, vector.element_size);
    }
    return vector;
}

VAPI void Vector_destroy(Vector *vector){
    if(!vector || !vector->data) return;

    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = 0;
    return;
}



VAPI void Vector_push(Vector *vector, const void *value){
    if(!vector || !vector->element_size) return;

    if(!vector->data){
        *vector = Vector_create(vector->element_size);
    }

    else if(vector->size >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
    }
    memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size);
    vector->size++;
    return;
}

VAPI void Vector_pushArray(Vector *vector, const void *value, ssize_t amount){
    if(!vector || !vector->element_size || amount <= 0) return;

    if(!vector->data){
        *vector = Vector_create(vector->element_size);
    }

    else if(vector->size + amount > vector->capacity){
        vector->capacity *= 2;
        vector->capacity = vector->capacity > (vector->size + amount) ? vector->capacity : (vector->size + amount + 10);
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
    }
    memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size * amount);
    vector->size++;
    return;
}

VAPI int Vector_pop(Vector *vector, void *ret){
    if(!vector || !vector->data) return 0;

    if(vector->size == 0){
        return 0;
    }
    vector->size--;
    if(ret == NULL) {
        return 0;
    }
    memcpy(ret, (char*)vector->data + (vector->size * vector->element_size), vector->element_size);
    return 1;
}

VAPI void Vector_insert(Vector *vector, const ssize_t index, const void *value){
    if(!vector || !vector->element_size || index < 0) return;
    
    size_t idx = (size_t) index;

    if(idx > vector->size){
        return;
    }
    else if(idx == vector->size){
        Vector_push(vector, value);
        return;
    }
    Vector_shiftRightFrom(vector, idx);
    memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    return;
}

VAPI void Vector_set(Vector *vector, const ssize_t index, const void *value){
    if(!vector || !vector->data) return;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){    //idx is size_t -> unsigned, cannot be less than 0, but size is also size_t, that's why we compare size with idx, and index with less than one
        return;
    }
    memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    return;
}

VAPI int Vector_get(Vector *vector, const ssize_t index, void *ret){
    if(!vector || !vector->data) return 0;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return 0;
    }
    memcpy(ret, (char*)vector->data + (idx * vector->element_size), vector->element_size);
    return 1;
}

void *Vector_getPointer(Vector *vector, const ssize_t index){
    if(!vector || !vector->data) return NULL;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return NULL;
    }
    return (char*)vector->data + (index * vector->element_size);
}

VAPI void Vector_clear(Vector *vector){
    if(!vector || !vector->data) return;
    
    vector->size = 0;
    return;
}

VAPI void Vector_resize(Vector *vector, ssize_t capacity){
    if(!vector || capacity < 0 || !vector->element_size) return;

    size_t cap = (size_t) capacity;
    if(!vector->data){
        *vector = Vector_createCapacity(cap, vector->element_size);
        return;
    }

    if(vector->capacity == cap) return;
    else if(vector->size >= cap) vector->size = cap;
    vector->capacity = cap;
    vector->data = realloc(vector->data, vector->capacity * vector->element_size);
    return;
}


VAPI Vector Vector_copyReturn(Vector *src){
    if(!src) return VECTOR_NULL();

    Vector vector;
    vector.size = src->size;
    vector.capacity = src->capacity;
    vector.element_size = src->element_size;
    vector.data = malloc(src->capacity * src->element_size);
    memcpy(vector.data, src->data, src->size * src->element_size);
    return vector;
}

VAPI void Vector_copy(Vector *dest, Vector *src){
    if(!dest || !src) return;

    // if(dest->data != NULL){
    //     free(dest->data);
    // }
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->element_size = src->element_size;
    dest->data = malloc(src->capacity * src->element_size);
    memcpy(dest->data, src->data, src->size * src->element_size);
    return;
}



VAPI size_t Vector_getCapacity(Vector *vector){
    if(vector) return vector->capacity;
    return 0;
}

VAPI size_t Vector_getSize(Vector *vector){
    if(vector) return vector->size;
    return 0;
}

VAPI int Vector_getSizeOfElements(Vector *vector){
    if(vector) return vector->element_size;
    return 0;
}



VAPI void Vector_shiftRightFrom(Vector *vector, const ssize_t index){
    if(!vector || !vector->size) return;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + 1) >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + vector->element_size, src, (vector->size - idx) * vector->element_size);
    
    vector->size++;
    return;
}

VAPI void Vector_shiftLeftFrom(Vector *vector, const ssize_t index){
    if(!vector || !vector->size) return;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }

    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + vector->element_size, idx * vector->element_size);
    
    if(idx == (vector->size-1)) vector->size--;
    return;
}

VAPI void Vector_shiftRightFromBy(Vector *vector, const ssize_t index, ssize_t amount){
    if(!vector || amount <= 0 || !vector->size) return;

    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + amt) >= vector->capacity){
        vector->capacity = (vector->size + amt) * 1.5;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + amt * vector->element_size, src, (vector->size - idx) * vector->element_size);
    
    vector->size += amt;
    return;
}

VAPI void Vector_shiftLeftFromBy(Vector *vector, const ssize_t index, ssize_t amount){
    if(!vector || amount <= 0 || !vector->size) return;
    
    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }

    size_t copy_amount = idx >= amt ? idx - amt + 1 : 0;
    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + amt * vector->element_size, copy_amount * vector->element_size);
    
    if(idx == (vector->size-1)) vector->size = vector->size > amt ? vector->size - amt : 0;
    return;
}



VAPI void Vector_sort(const Vector *vector, const int (*Vector_compare)(const void *a, const void *b)){
    if(!vector || !vector->size) return;

    qsort(vector->data, vector->size, vector->element_size, Vector_compare);
    return;
}



//------------------------------------------------------------------------------------------------------------------



VAPI CVector CVector_create(int element_size, CVdestroy_func destroy, CVcopy_func copy){
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

VAPI CVector CVector_createDefault1D(int element_size){
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

VAPI CVector CVector_createDefault2D(int element_size){
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

VAPI CVector CVector_createCapacity(ssize_t capacity, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if(capacity <= 0) return _CVECTOR_EMPTY(element_size, destroy, copy);

    size_t cap = (size_t) capacity;
    CVector vector;
    vector.size = 0;
    vector.capacity = cap;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

VAPI CVector CVector_createSize(ssize_t size, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if (size < 0) return _CVECTOR_EMPTY(element_size, destroy, copy);
    
    size_t sz = (size_t) size;
    CVector vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

VAPI CVector CVector_createSizeSet(ssize_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if (size < 0) return _CVECTOR_EMPTY(element_size, destroy, copy);

    size_t sz = (size_t) size;
    CVector vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * vector.element_size), value, vector.element_size);
    }
    return vector;
}

VAPI CVector CVector_createCapacitySizeSet(ssize_t capacity, ssize_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if (capacity < 0) return _CVECTOR_EMPTY(element_size, destroy, copy);
    else if (capacity < size) size = capacity;
    else if(size < 0) size = 0;
    
    size_t cap = (size_t) capacity;
    size_t sz = (size_t) size;

    CVector vector;
    vector.size = sz;
    vector.capacity = cap >= sz ? cap : sz;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = Vector_destroy_wrapper;
    vector.copy = Vector_copy_wrapper;
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * vector.element_size), value, vector.element_size);
    }
    return vector;
}

VAPI void CVector_destroy(CVector *vector){
    if(!vector || !vector->data) return;
    if(vector->destroy == NULL){
        goto free_top;
    }

    for(size_t i = 0; i < vector->size; i++){
        vector->destroy((char*)vector->data + vector->element_size * i);
    }
    free_top:
    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->destroy = NULL;
    vector->copy = NULL;
    return;
}

VAPI void Vector_destroy_wrapper(void *element){
    Vector_destroy((Vector*)element);
    return;
}



VAPI void CVector_push(CVector *vector, const void *value){
    if(!vector || !vector->element_size || !value) return;

    if(!vector->data){
        *vector = CVector_create(vector->element_size, vector->destroy, vector->copy);
    }

    if(vector->size >= vector->capacity){
        size_t old_capacity = vector->capacity;
        vector->capacity *= 2;
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
        memset((char*)vector->data + old_capacity * vector->element_size, 0, old_capacity * vector->element_size);
    }
    if(vector->copy) vector->copy((char*)vector->data + (vector->size * vector->element_size), value);
    else memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size);
    vector->size++;
    return;
}

VAPI int CVector_pop(CVector *vector, void *ret){
    if(!vector || !vector->data) return 0;

    if(vector->size == 0){
        return 0;
    }
    vector->size--;
    if(ret == NULL) {
        //destroy the element that is getting popped but not assigned to a variable
        if(vector->destroy) vector->destroy((char*)vector->data + vector->size * vector->element_size);
        return 0;
    }
    memcpy(ret, (char*)vector->data + (vector->size * vector->element_size), vector->element_size);
    memset((char*)vector->data + vector->size * vector->element_size, 0, vector->element_size);
    return 1;
}


VAPI void CVector_insert(CVector *vector, const ssize_t index, const void *value){
    if(!vector || !vector->element_size || index < 0) return;

    size_t idx = (size_t) index;

    if(idx > vector->size){
        return;
    }
    else if(idx == vector->size){
        CVector_push(vector, value);
        return;
    }
    CVector_shiftRightFrom(vector, idx);

    if(vector->copy) vector->copy((char*)vector->data + (idx * vector->element_size), value);
    else memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    
    return;
}

VAPI void CVector_set(CVector *vector, const ssize_t index, const void *value){
    if(!vector || !vector->data) return;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }

    if(vector->copy) vector->copy((char*)vector->data + (idx * vector->element_size), value);
    else memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    return;
}

VAPI int CVector_get(CVector *vector, const ssize_t index, void *ret){
    if(!vector || !vector->data) return 0;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return 0;
    }
    if(vector->copy) vector->copy(ret, (char*)vector->data + (idx * vector->element_size));
    else memcpy(ret, (char*)vector->data + (idx * vector->element_size), vector->element_size);
    return 1;
}

VAPI int CVector_getReference(CVector *vector, const ssize_t index, void *ret){
    if(!vector || !vector->size) return 0;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return 0;
    }
    if(vector->copy) vector->copy(ret, (char*)vector->data + (idx * vector->element_size));
    else memcpy(ret, (char*)vector->data + (idx * vector->element_size), vector->element_size);
    return 1;
}

void *CVector_getPointer(CVector *vector, const ssize_t index){
    if(!vector || !vector->data) return NULL;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return NULL;
    }
    return (char*)vector->data + (idx * vector->element_size);
}

VAPI void CVector_clear(CVector *vector){
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

VAPI void CVector_resize(CVector *vector, ssize_t capacity){
    if(!vector || capacity < 0 || !vector->element_size) return;
    
    size_t cap = (size_t) capacity;
    if(!vector->data){
        *vector = CVector_createCapacity(cap, vector->element_size, vector->destroy, vector->copy);
        return;
    }

    if(vector->capacity == cap) return;
    else if(vector->size >= cap){
        if(vector->destroy){
            for(size_t i = cap; i < vector->size; i++){
                vector->destroy((char*)vector->data + vector->element_size * i);
            }
        }    
        vector->size = cap;
    }
    vector->capacity = cap;
    vector->data = realloc(vector->data, vector->capacity * vector->element_size);
    return;
}



VAPI CVector CVector_copyReturn(CVector *src){
    if(!src) return CVECTOR_NULL();

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


VAPI void CVector_copy(CVector *dest, CVector *src){
    if(!dest || !src) return;

    // if(dest->data != NULL){
    //     CVector_destroy(dest);
    // }
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

VAPI void Vector_copy_wrapper(void *dst, const void *src){
    Vector_copy((Vector*)dst, (Vector*)src);
}



VAPI size_t CVector_getCapacity(CVector *vector){
    if(vector) return vector->capacity;
    return 0;
}

VAPI size_t CVector_getSize(CVector *vector){
    if(vector) return vector->size;
    return 0;
}

VAPI int CVector_getSizeOfElements(CVector *vector){
    if(vector) return vector->element_size;
    return 0;
}


VAPI void CVector_shiftRightFrom(CVector *vector, const ssize_t index){
    if(!vector || !vector->size) return;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + 1) >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + vector->element_size, src, (vector->size - idx) * vector->element_size);
    memset(src, 0, vector->element_size);

    vector->size++;
    return;
}

VAPI void CVector_shiftLeftFrom(CVector *vector, const ssize_t index){
    if(!vector || !vector->size) return;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }

    if(vector->destroy) vector->destroy(vector->data);
    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + vector->element_size, idx * vector->element_size);
    memset((char*)src + idx * vector->element_size, 0, vector->element_size);

    if(idx == (vector->size-1)) vector->size--;
    return;
}

VAPI void CVector_shiftRightFromBy(CVector *vector, const ssize_t index, ssize_t amount){
    if(!vector || amount <= 0 || !vector->size) return;


    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + amt) >= vector->capacity){
        vector->capacity = (vector->size + amt) * 1.5;
        vector->data = realloc(vector->data, vector->capacity);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + amt * vector->element_size, src, (vector->size - idx) * vector->element_size);
    memset(src, 0, amt * vector->element_size);

    vector->size += amt;
    return;
}

VAPI void CVector_shiftLeftFromBy(CVector *vector, const ssize_t index, ssize_t amount){
    if(!vector || amount <= 0 || !vector->size) return;
    
    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }

    size_t copy_amount = idx >= amt ? idx - amt + 1 : 0;
    size_t destroy_amount = idx + 1 - copy_amount;
    if(vector->destroy){
        char *destroy_add = (char*)vector->data;
        for(size_t i = 0; i < destroy_amount; i++){
            vector->destroy(destroy_add);
            destroy_add += vector->element_size;
        }
    }
    void *src = (char*)vector->data;// + (index) * vector->element_size;
    memmove(src, (char*)src + amt * vector->element_size, copy_amount * vector->element_size);
    memset((char*)src + copy_amount * vector->element_size, 0, destroy_amount * vector->element_size);

    if(idx == (vector->size-1)) vector->size = vector->size > amt ? vector->size - amt : 0;
    return;
}



VAPI void CVector_sort(const CVector *vector, const int (*CVector_compare)(const void *a, const void *b)){
    if(!vector || !vector->size) return;

    qsort(vector->data, vector->size, vector->element_size, CVector_compare);
    return;
}


// #endif