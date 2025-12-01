#ifndef VECTOR_C
#define VECTOR_C
#include "../vector.h"


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

void Vector_destroy(Vector *vector){
    free(vector->data);
    return;
}



void Vector_push(Vector *vector, const void *value){
    if(vector->size >= vector->capacity){
        vector->capacity *= 2;
        vector->data = realloc(vector->data, (vector->capacity * vector->element_size));
    }
    memcpy((char*)vector->data + (vector->size * vector->element_size), value, vector->element_size);
    vector->size++;
    return;
}

void Vector_pop(Vector *vector, void *ret){
    if(vector->size == 0){
        return;
    }
    vector->size--;
    memcpy(ret, (char*)vector->data + (vector->size * vector->element_size), vector->element_size);
    return;
}

void Vector_insert(Vector *vector, const size_t index, const void *value){
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
    if(index >= vector->size || index < -1){
        return;
    }
    memcpy((char*)vector->data + (index * vector->element_size), value, vector->element_size);
    return;
}

void Vector_get(Vector *vector, const size_t index, void *ret){
    if(index >= vector->size){
        return;
    }
    memcpy(ret, (char*)vector->data + (index * vector->element_size), vector->element_size);
    return;
}

void Vector_clear(Vector *vector){
    vector->size = 0;
    return;
}


Vector Vector_copyReturn(Vector *src){
    Vector vector;
    vector.size = src->size;
    vector.capacity = src->capacity;
    vector.element_size = src->element_size;
    vector.data = malloc(src->capacity * src->element_size);
    memcpy(vector.data, src->data, src->capacity * src->element_size);
    return vector;
}

void Vector_copy(Vector *dest, Vector *src){
    if(dest->data != NULL){
        free(dest->data);
    }
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->element_size = src->element_size;
    dest->data = malloc(src->capacity * src->element_size);
    memcpy(dest->data, src->data, src->capacity * src->element_size);
    return;
}


void Vector_shiftRightFrom(Vector *vector, const size_t index){
    if(index >= vector->size){
        return;
    }
    else if((vector->size + 1) >= vector->capacity){
        vector->capacity *= 2;
    }

    void *src = (char*)vector->data + (index) * vector->element_size;
    memmove((char*)src + vector->element_size, src, vector->size - index);
    
    vector->size++;
    return;
}

void Vector_shiftLeftFrom(Vector *vector, const size_t index){
    if(index >= vector->size || index <= 0){
        return;
    }

    void *src = (char*)vector->data + (index) * vector->element_size;
    memmove((char*)src - vector->element_size, src, index);
    
    if(index == (vector->size-1)) vector->size--;
    return;
}



long long int Vector_getCapacity(Vector *vector){
    if(vector) return vector->capacity;
}

long long int Vector_getSize(Vector *vector){
    if(vector) return vector->size;
}

long long int Vector_getSizeOfElements(Vector *vector){
    if(vector) return vector->element_size;
}


void Vector_sort(const Vector *vector, const int (*Vector_compare)(const void *a, const void *b)){
    qsort(vector->data, vector->size, vector->element_size, Vector_compare);
    return;
}



#endif