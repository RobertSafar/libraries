#ifndef RS_HASH_TABLE_H
#define RS_HASH_TABLE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define RSHASH_STATE_MASK ((uint8_t)(0b11 << 6))

typedef size_t (*rsHashFunction)(void *key);
typedef bool (*rsHashCompare)(void *key1, void *key2);

typedef void (*rsHashDelete)(void *key, void *value);
typedef void (*rsHashCopy)(void *slot_key, void *slot_value, void *key, void *value);

typedef struct RSHash{
    size_t size;
    size_t capacity;
    unsigned int key_size;
    unsigned int value_size;
    
    uint8_t *meta;
    void *entries;

    rsHashCompare compare;
    rsHashFunction hash;
    rsHashCopy copy;
    rsHashDelete delete;

}RSHash;

typedef enum{
    RSHASH_EMPTY,
    RSHASH_TAKEN,
    RSHASH_DELETED
}RSSlotState;

RSHash RShash_create(unsigned int key_size, unsigned int value_size, rsHashCompare compare, rsHashFunction hash, rsHashCopy copy, rsHashDelete delete);

RSHash RShash_createCapacity(unsigned int key_size, unsigned int value_size, rsHashCompare compare, rsHashFunction hash, size_t capacity, rsHashCopy copy, rsHashDelete delete);

void RShash_destroy(RSHash *table);

void RShash_clear(RSHash *table);

void *RShash_search(RSHash *table, void *key);

void *RShash_insert(RSHash *table, void *key, void *value);

void RShash_delete(RSHash *table, void *key);

void *RShash_getKey(RSHash *table, size_t index);

void *RShash_getNextKey(RSHash *table, void *key);

void *RShash_getValue(RSHash *table, void *key);

RSSlotState RShash_getSlotState(RSHash *table, size_t index);

size_t RShash_getSize(RSHash *table);

size_t RShash_getCapacity(RSHash *table);

#endif