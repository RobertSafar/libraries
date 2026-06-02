#include "rsVector.h"

#ifdef _WIN32
    #ifdef BUILDING_DLL
        #define RSAPI __declspec(dllexport)
    #elif defined(USING_DLL)
        #define RSAPI __declspec(dllimport)
    #else
        #define RSAPI
    #endif
#else
  #define RSAPI   /* nothing */
#endif



RSAPI RSVec RSvec_create(int element_size){
    RSVec vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

RSAPI RSVec RSvec_createCapacity(ssize_t capacity, int element_size){
    if (capacity < 0) return _VECTOR_EMPTY(element_size);

    size_t cap = (size_t) capacity;
    RSVec vector;
    vector.size = 0;
    vector.capacity = cap;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    return vector;
}

RSAPI RSVec RSvec_createSize(ssize_t size, int element_size){
    if (size < 0) return _VECTOR_EMPTY(element_size);

    size_t sz = (size_t) size;
    RSVec vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    memset(vector.data, 0, (vector.size * vector.element_size));
    return vector;
}

RSAPI RSVec RSvec_createSizeSet(ssize_t size, void *value, int element_size){
    if (size < 0) return _VECTOR_EMPTY(element_size);

    size_t sz = (size_t) size;
    RSVec vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * element_size), value, element_size);
    }
    return vector;
}

RSAPI RSVec RSvec_createCapacitySizeSet(ssize_t capacity, ssize_t size, void *value, int element_size){    
    if (capacity < 0) return _VECTOR_EMPTY(element_size);
    else if (capacity < size) size = capacity;
    else if (size < 0) size = 0;
    
    size_t cap = (size_t) capacity;
    size_t sz = (size_t) size;
    
    RSVec vector;
    vector.size = sz;
    vector.capacity = cap >= sz ? cap : sz;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * vector.element_size), value, vector.element_size);
    }
    return vector;
}

RSAPI void RSvec_destroy(RSVec *vector){
    if(!vector || !vector->capacity) return;

    free(vector->data);
    vector->data = NULL;
    vector->size = 0;
    vector->capacity = 0;
    vector->element_size = 0;
    return;
}



RSAPI void RSvec_push(RSVec *vector, const void *value){
    if(!vector || !vector->element_size) return;

    if(!vector->capacity){
        *vector = RSvec_create(vector->element_size);
    }

    else if(vector->size >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
    }
    memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size);
    vector->size++;
    return;
}

RSAPI void RSvec_pushArray(RSVec *vector, const void *value, ssize_t amount){
    if(!vector || !vector->element_size || amount <= 0) return;

    if(!vector->capacity){
        *vector = RSvec_create(vector->element_size);
    }

    else if(vector->size + amount > vector->capacity){
        vector->capacity *= 2;
        vector->capacity = vector->capacity > (vector->size + amount) ? vector->capacity : (vector->size + amount + 10);
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
    }
    memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size * amount);
    vector->size += amount;
    return;
}

RSAPI int RSvec_pop(RSVec *vector, void *ret){
    if(!vector || !vector->size) return 0;

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

RSAPI void RSvec_insert(RSVec *vector, const ssize_t index, const void *value){
    if(!vector || !vector->element_size || index < 0) return;
    
    size_t idx = (size_t) index;

    if(idx > vector->size){
        return;
    }
    else if(idx == vector->size){
        RSvec_push(vector, value);
        return;
    }
    RSvec_shiftRightFrom(vector, idx);
    memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    return;
}

RSAPI void RSvec_set(RSVec *vector, const ssize_t index, const void *value){
    if(!vector || !vector->size) return;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){    //idx is size_t -> unsigned, cannot be less than 0, but size is also size_t, that's why we compare size with idx, and index with less than one
        return;
    }
    memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    return;
}

RSAPI int RSvec_get(RSVec *vector, const ssize_t index, void *ret){
    if(!vector || !vector->size) return 0;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return 0;
    }
    memcpy(ret, (char*)vector->data + (idx * vector->element_size), vector->element_size);
    return 1;
}

void *RSvec_getPointer(RSVec *vector, const ssize_t index){
    if(!vector || !vector->size) return NULL;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return NULL;
    }
    return (char*)vector->data + (index * vector->element_size);
}

RSAPI void RSvec_clear(RSVec *vector){
    if(!vector || !vector->size) return;
    
    vector->size = 0;
    return;
}

RSAPI void RSvec_resize(RSVec *vector, ssize_t capacity){
    if(!vector || capacity < 0 || !vector->element_size) return;

    size_t cap = (size_t) capacity;
    if(!vector->capacity){
        *vector = RSvec_createCapacity(cap, vector->element_size);
        return;
    }

    if(vector->capacity == cap) return;
    else if(vector->size >= cap) vector->size = cap;
    vector->capacity = cap;
    vector->data = realloc(vector->data, vector->capacity * vector->element_size);
    return;
}


RSAPI RSVec RSvec_copyReturn(RSVec *src){
    if(!src) return VECTOR_NULL();

    RSVec vector;
    vector.size = src->size;
    vector.capacity = src->capacity;
    vector.element_size = src->element_size;
    if(src->capacity * src->element_size){
        vector.data = malloc(src->capacity * src->element_size);
    }
    else{
        vector.data = NULL;
    }
    memcpy(vector.data, src->data, src->size * src->element_size);
    return vector;
}

RSAPI void RSvec_copy(RSVec *dest, RSVec *src){
    if(!dest || !src) return;

    // if(dest->data != NULL){
    //     free(dest->data);
    // }
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->element_size = src->element_size;
    if(src->capacity * src->element_size){
        dest->data = malloc(src->capacity * src->element_size);
    }
    else{
        dest->data = NULL;
    }
    memcpy(dest->data, src->data, src->size * src->element_size);
    return;
}



RSAPI size_t RSvec_getCapacity(RSVec *vector){
    if(vector) return vector->capacity;
    return 0;
}

RSAPI size_t RSvec_getSize(RSVec *vector){
    if(vector) return vector->size;
    return 0;
}

RSAPI int RSvec_getSizeOfElements(RSVec *vector){
    if(vector) return vector->element_size;
    return 0;
}



RSAPI void RSvec_shiftRightFrom(RSVec *vector, const ssize_t index){
    if(!vector || !vector->size) return;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + 1) >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity * vector->element_size);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + vector->element_size, src, (vector->size - idx) * vector->element_size);
    
    vector->size++;
    return;
}

RSAPI void RSvec_shiftLeftFrom(RSVec *vector, const ssize_t index){
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

RSAPI void RSvec_shiftRightFromBy(RSVec *vector, const ssize_t index, ssize_t amount){
    if(!vector || amount <= 0 || !vector->size) return;

    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + amt) >= vector->capacity){
        vector->capacity = (vector->size + amt) * 1.5;
        vector->data = realloc(vector->data, vector->capacity * vector->element_size);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + amt * vector->element_size, src, (vector->size - idx) * vector->element_size);
    
    vector->size += amt;
    return;
}

RSAPI void RSvec_shiftLeftFromBy(RSVec *vector, const ssize_t index, ssize_t amount){
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



RSAPI void RSvec_sort(const RSVec *vector, const int (*RSvec_compare)(const void *a, const void *b)){
    if(!vector || !vector->size) return;

    qsort(vector->data, vector->size, vector->element_size, RSvec_compare);
    return;
}



//------------------------------------------------------------------------------------------------------------------



RSAPI RSManagedVec RSmvec_create(int element_size, CVdestroy_func destroy, CVcopy_func copy){
    RSManagedVec vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = destroy;
    vector.copy = copy;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

RSAPI RSManagedVec RSmvec_createDefault1D(int element_size){
    RSManagedVec vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = NULL;
    vector.copy = NULL;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

RSAPI RSManagedVec RSmvec_createDefault2D(int element_size){
    RSManagedVec vector;
    vector.size = 0;
    vector.capacity = 10;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = RSvec_destroy_wrapper;
    vector.copy = RSvec_copy_wrapper;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

RSAPI RSManagedVec RSmvec_createCapacity(ssize_t capacity, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if(capacity <= 0) return _CVECTOR_EMPTY(element_size, destroy, copy);

    size_t cap = (size_t) capacity;
    RSManagedVec vector;
    vector.size = 0;
    vector.capacity = cap;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);

    vector.destroy = destroy;
    vector.copy = copy;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

RSAPI RSManagedVec RSmvec_createSize(ssize_t size, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if (size < 0) return _CVECTOR_EMPTY(element_size, destroy, copy);
    
    size_t sz = (size_t) size;
    RSManagedVec vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(vector.element_size * vector.capacity);

    vector.destroy = destroy;
    vector.copy = copy;
    memset(vector.data, 0, (vector.capacity * vector.element_size));
    return vector;
}

RSAPI RSManagedVec RSmvec_createSizeSet(ssize_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if (size < 0) return _CVECTOR_EMPTY(element_size, destroy, copy);

    size_t sz = (size_t) size;
    RSManagedVec vector;
    vector.size = sz;
    vector.capacity = sz;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = destroy;
    vector.copy = copy;
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * vector.element_size), value, vector.element_size);
    }
    return vector;
}

RSAPI RSManagedVec RSmvec_createCapacitySizeSet(ssize_t capacity, ssize_t size, void *value, int element_size, CVdestroy_func destroy, CVcopy_func copy){
    if (capacity < 0) return _CVECTOR_EMPTY(element_size, destroy, copy);
    else if (capacity < size) size = capacity;
    else if(size < 0) size = 0;
    
    size_t cap = (size_t) capacity;
    size_t sz = (size_t) size;

    RSManagedVec vector;
    vector.size = sz;
    vector.capacity = cap >= sz ? cap : sz;
    vector.element_size = element_size;
    vector.data = malloc(element_size * vector.capacity);

    vector.destroy = destroy;
    vector.copy = copy;
    for(size_t i=0; i<sz; i++){
        memcpy((char*)vector.data + (i * vector.element_size), value, vector.element_size);
    }
    return vector;
}

RSAPI void RSmvec_destroy(RSManagedVec *vector){
    if(!vector || !vector->capacity) return;
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

RSAPI void RSvec_destroy_wrapper(void *element){
    RSvec_destroy((RSVec*)element);
    return;
}



RSAPI void RSmvec_push(RSManagedVec *vector, const void *value){
    if(!vector || !vector->element_size || !value) return;

    if(!vector->capacity){
        *vector = RSmvec_create(vector->element_size, vector->destroy, vector->copy);
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

RSAPI int RSmvec_pop(RSManagedVec *vector, void *ret){
    if(!vector || !vector->size) return 0;

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


RSAPI void RSmvec_insert(RSManagedVec *vector, const ssize_t index, const void *value){
    if(!vector || !vector->element_size || index < 0) return;

    size_t idx = (size_t) index;

    if(idx > vector->size){
        return;
    }
    else if(idx == vector->size){
        RSmvec_push(vector, value);
        return;
    }
    RSmvec_shiftRightFrom(vector, idx);

    if(vector->copy) vector->copy((char*)vector->data + (idx * vector->element_size), value);
    else memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    
    return;
}

RSAPI void RSmvec_set(RSManagedVec *vector, const ssize_t index, const void *value){
    if(!vector || !vector->size) return;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }

    if(vector->copy) vector->copy((char*)vector->data + (idx * vector->element_size), value);
    else memcpy((char*)vector->data + (idx * vector->element_size), value, vector->element_size);
    return;
}

RSAPI int RSmvec_get(RSManagedVec *vector, const ssize_t index, void *ret){
    if(!vector || !vector->size) return 0;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return 0;
    }
    if(vector->copy) vector->copy(ret, (char*)vector->data + (idx * vector->element_size));
    else memcpy(ret, (char*)vector->data + (idx * vector->element_size), vector->element_size);
    return 1;
}

RSAPI int RSmvec_getReference(RSManagedVec *vector, const ssize_t index, void *ret){
    if(!vector || !vector->size) return 0;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return 0;
    }
    if(vector->copy) vector->copy(ret, (char*)vector->data + (idx * vector->element_size));
    else memcpy(ret, (char*)vector->data + (idx * vector->element_size), vector->element_size);
    return 1;
}

void *RSmvec_getPointer(RSManagedVec *vector, const ssize_t index){
    if(!vector || !vector->size) return NULL;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return NULL;
    }
    return (char*)vector->data + (idx * vector->element_size);
}

RSAPI void RSmvec_clear(RSManagedVec *vector){
    if(!vector || !vector->size) return;
    
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

RSAPI void RSmvec_resize(RSManagedVec *vector, ssize_t capacity){
    if(!vector || capacity < 0 || !vector->element_size) return;
    
    size_t cap = (size_t) capacity;
    if(!vector->capacity){
        *vector = RSmvec_createCapacity(cap, vector->element_size, vector->destroy, vector->copy);
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



RSAPI RSManagedVec RSmvec_copyReturn(RSManagedVec *src){
    if(!src) return CVECTOR_NULL();

    RSManagedVec vector;
    vector.size = src->size;
    vector.capacity = src->capacity;
    vector.element_size = src->element_size;
    vector.copy = src->copy;
    vector.destroy = src->destroy;
    if(src->capacity * src->element_size){
        vector.data = malloc(src->capacity * src->element_size);
    }
    else{
        vector.data = NULL;
    }

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


RSAPI void RSmvec_copy(RSManagedVec *dest, RSManagedVec *src){
    if(!dest || !src) return;

    
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->element_size = src->element_size;
    dest->copy = src->copy;
    dest->destroy = src->destroy;
    if(src->capacity * src->element_size){
        dest->data = malloc(src->capacity * src->element_size);
    }
    else{
        dest->data = NULL;
    }

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

RSAPI void RSvec_copy_wrapper(void *dst, const void *src){
    RSvec_copy((RSVec*)dst, (RSVec*)src);
}



RSAPI size_t RSmvec_getCapacity(RSManagedVec *vector){
    if(vector) return vector->capacity;
    return 0;
}

RSAPI size_t RSmvec_getSize(RSManagedVec *vector){
    if(vector) return vector->size;
    return 0;
}

RSAPI int RSmvec_getSizeOfElements(RSManagedVec *vector){
    if(vector) return vector->element_size;
    return 0;
}


RSAPI void RSmvec_shiftRightFrom(RSManagedVec *vector, const ssize_t index){
    if(!vector || !vector->size) return;

    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + 1) >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, vector->capacity * vector->element_size);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + vector->element_size, src, (vector->size - idx) * vector->element_size);
    memset(src, 0, vector->element_size);

    vector->size++;
    return;
}

RSAPI void RSmvec_shiftLeftFrom(RSManagedVec *vector, const ssize_t index){
    if(!vector || !vector->size) return;
    
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }

    if(vector->destroy) vector->destroy(vector->data);
    void *src = (char*)vector->data;
    memmove(src, (char*)src + vector->element_size, idx * vector->element_size);
    memset((char*)src + idx * vector->element_size, 0, vector->element_size);

    if(idx == (vector->size-1)) vector->size--;
    return;
}

RSAPI void RSmvec_shiftRightFromBy(RSManagedVec *vector, const ssize_t index, ssize_t amount){
    if(!vector || amount <= 0 || !vector->size) return;


    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= vector->size || index < 0){
        return;
    }
    else if((vector->size + amt) >= vector->capacity){
        vector->capacity = (vector->size + amt) * 1.5;
        vector->data = realloc(vector->data, vector->capacity * vector->element_size);
    }

    void *src = (char*)vector->data + (idx) * vector->element_size;
    memmove((char*)src + amt * vector->element_size, src, (vector->size - idx) * vector->element_size);
    memset(src, 0, amt * vector->element_size);

    vector->size += amt;
    return;
}

RSAPI void RSmvec_shiftLeftFromBy(RSManagedVec *vector, const ssize_t index, ssize_t amount){
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
    void *src = (char*)vector->data;
    memmove(src, (char*)src + amt * vector->element_size, copy_amount * vector->element_size);
    memset((char*)src + copy_amount * vector->element_size, 0, destroy_amount * vector->element_size);

    if(idx == (vector->size-1)) vector->size = vector->size > amt ? vector->size - amt : 0;
    return;
}



RSAPI void RSmvec_sort(const RSManagedVec *vector, const int (*RSmvec_compare)(const void *a, const void *b)){
    if(!vector || !vector->size) return;

    qsort(vector->data, vector->size, vector->element_size, RSmvec_compare);
    return;
}