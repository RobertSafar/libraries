
#include "rsHashTable.h"


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

#define TAKE_FINGERPRINT(index) ((index >> (64 - 48)) & (~RSHASH_STATE_MASK))

RSAPI RSHash RShash_create(unsigned int key_size, unsigned int value_size, rsHashCompare compare, rsHashFunction hash, rsHashCopy copy, rsHashDelete delete){
    RSHash retval = {
        .size = 0,
        .capacity = 10,
        .key_size = key_size,
        .value_size = value_size,
        .compare = compare,
        .hash = hash,
        .copy = copy,
        .delete = delete
    };
    
    size_t meta_bytes = retval.capacity * sizeof(uint8_t);
    size_t entry_bytes = retval.capacity * (retval.key_size + retval.value_size);
    retval.meta = (uint8_t*)malloc(meta_bytes + entry_bytes);
    retval.entries = (void*)(retval.meta + meta_bytes);
    memset(retval.meta, 0, meta_bytes);
    return retval;
}

RSAPI RSHash RShash_createCapacity(unsigned int key_size, unsigned int value_size, rsHashCompare compare, rsHashFunction hash, size_t capacity, rsHashCopy copy, rsHashDelete delete){
    RSHash retval = {
        .size = 0,
        .capacity = capacity,
        .key_size = key_size,
        .value_size = value_size,
        .compare = compare,
        .hash = hash,
        .copy = copy,
        .delete = delete
    };
    
    size_t meta_bytes = retval.capacity * sizeof(uint8_t);
    size_t entry_bytes = retval.capacity * (retval.key_size + retval.value_size);
    retval.meta = (uint8_t*)malloc(meta_bytes + entry_bytes);
    retval.entries = (void*)(retval.meta + meta_bytes);
    memset(retval.meta, 0, meta_bytes);
    return retval;
}

RSAPI void RShash_destroy(RSHash *table){
    if(table->delete){
        unsigned int slot_size = table->key_size + table->value_size;
        size_t deleted_cnt = 0;
        for(size_t i = 0; i < table->capacity; i++){
            RSSlotState state = (table->meta[i] & RSHASH_STATE_MASK) >> 6;

            if(state == RSHASH_TAKEN){
                void *key = (char*)table->entries + i * slot_size;
                table->delete(key, (char*)key + table->key_size);
                deleted_cnt++;
            }

            if(deleted_cnt == table->size) break;
        }
    }

    free(table->meta);

    memset(table, 0, sizeof(RSHash));

    return;
}

RSAPI void RShash_clear(RSHash *table){
    if(table->delete){
        unsigned int slot_size = table->key_size + table->value_size;
        size_t deleted_cnt = 0;
        for(size_t i = 0; i < table->capacity; i++){
            RSSlotState state = (table->meta[i] & RSHASH_STATE_MASK) >> 6;
            
            if(state == RSHASH_TAKEN){
                void *key = (char*)table->entries + i * slot_size;
                table->delete(key, (char*)key + table->key_size);
                deleted_cnt++;
            }
            
            if(deleted_cnt == table->size) break;
        }
    }
    table->size = 0;
    memset(table->meta, 0, table->capacity);
    return;
}

static void resize(RSHash *table){

    RSHash new_table = {
        .size = table->size,
        .capacity = table->capacity * 2,
        .key_size = table->key_size,
        .value_size = table->value_size,
        .compare = table->compare,
        .hash = table->hash,
        .copy = table->copy,
        .delete = table->delete
    };
    size_t meta_bytes = new_table.capacity * sizeof(uint8_t);
    size_t entry_bytes = new_table.capacity * (new_table.key_size + new_table.value_size);
    new_table.meta = (uint8_t*)malloc(meta_bytes + entry_bytes);
    new_table.entries = (void*)(new_table.meta + meta_bytes);
    memset(new_table.meta, 0, meta_bytes);

    
    unsigned int slot_size = table->key_size + table->value_size;
    size_t rehashed_count = 0;
    for(size_t j = 0; j < table->capacity; j++){

        RSSlotState state = (table->meta[j] & RSHASH_STATE_MASK) >> 6;
    
        switch (state)
        {
        case RSHASH_EMPTY:
        case RSHASH_DELETED:
            break;
        case RSHASH_TAKEN:
            void *curr_key = (char*)table->entries + j * slot_size;
            size_t new_index = table->hash(curr_key);
            uint8_t fingerprint = TAKE_FINGERPRINT(new_index);
            new_index %= new_table.capacity;
            size_t first_free = -1;

            
            for(size_t i = 0; i < new_table.capacity; i++){
                void *new_key = (char*)new_table.entries + new_index * slot_size;
                
                RSSlotState new_state = (new_table.meta[new_index] & RSHASH_STATE_MASK) >> 6;
                
                switch (new_state){
                case RSHASH_EMPTY:
                    new_table.meta[new_index] = fingerprint | (RSHASH_TAKEN << 6);
                    memcpy(new_key, curr_key, slot_size);
                    i = new_table.capacity;
                    break;
                default:
                    break;
                }

                new_index++;
                if(new_index >= new_table.capacity){
                    new_index = 0;
                }
            }
            
            rehashed_count++;
            break;
        default:
            break;
        }
        if(rehashed_count == table->size) break;
    }

    free(table->meta);
    *table = new_table;
    return;
}

static size_t probe(RSHash *table, void *key, RSSlotState *out_state){
    size_t index = table->hash(key);
    // uint8_t meta = (index >> (64 - 6)) & (~RSHASH_STATE_MASK);
    uint8_t meta = TAKE_FINGERPRINT(index);
    index  %= table->capacity;
    size_t first_free = -1;
    size_t entry_size = table->key_size + table->value_size;
    
    for(size_t i = 0; i < table->capacity; i++){
        RSSlotState state = (table->meta[index] & RSHASH_STATE_MASK) >> 6;
        
        switch (state)
        {
        case RSHASH_EMPTY:
            if(first_free == -1){
                *out_state = RSHASH_EMPTY;
                table->meta[index] = meta;
                return index;
            }
            *out_state = RSHASH_DELETED;
            table->meta[first_free] = meta | (RSHASH_DELETED << 6);
            return first_free;
            break;

        case RSHASH_TAKEN:
            void *slot_key = (char*)table->entries + index * entry_size;
            uint8_t fingerprint = (table->meta[index] & ~RSHASH_STATE_MASK);
            if(meta == fingerprint){

                if(table->compare(key, slot_key)){
                    *out_state = RSHASH_TAKEN;
                    return index;
                }
            
            }
            break;

        case RSHASH_DELETED:        
        default:
            if(first_free == -1){
                    first_free = index;
            }
            break;
        }

        index++;
        if(index >= table->capacity){
            index = 0;
        }
    }

    if(first_free == -1){
        //table full, shouldn't get here
    }

    *out_state = RSHASH_DELETED;
    return first_free;
}

RSAPI void *RShash_search(RSHash *table, void *key){
    RSSlotState state;
    size_t index = probe(table, key, &state);

    if(state != RSHASH_TAKEN) return NULL;

    return (char*)table->entries + index * (table->key_size + table->value_size) + table->key_size;
}

RSAPI void *RShash_insert(RSHash *table, void *key, void *value){

    if(table->size >= ((table->capacity * 3)>>2) ) {
        resize(table);
    } 

    RSSlotState state;
    size_t index = probe(table, key, &state);
    void *slot = (char*)table->entries + index * (table->key_size + table->value_size);

    if(state == RSHASH_TAKEN) {
        return (char*)slot + table->key_size;
    }
    
    table->size++;
    if(table->copy){
        table->copy(slot, (char*)slot + table->key_size, key, value);
    }
    else{
        memcpy(slot, key, table->key_size);
        memcpy((char*)slot + table->key_size, value, table->value_size);
    }
    table->meta[index] = (table->meta[index] & ~RSHASH_STATE_MASK) | (RSHASH_TAKEN << 6);

    return (char*)slot + table->key_size;
}

RSAPI void RShash_delete(RSHash *table, void *key){
    RSSlotState state;
    size_t index = probe(table, key, &state);
    
    if(state != RSHASH_TAKEN) return;
    
    void *slot = (char*)table->entries + index * (table->key_size + table->value_size);
    table->delete(slot, (char*)slot + table->key_size);
    
    table->size--;
    table->meta[index] = (table->meta[index] & ~RSHASH_STATE_MASK) | (RSHASH_DELETED << 6);

    return;
}

RSAPI void *RShash_getKey(RSHash *table, size_t index){
    return (char*)table->entries + index * (table->key_size + table->value_size);
}

RSAPI void *RShash_getNextKey(RSHash *table, void *key){
    unsigned int entry_size = table->key_size + table->value_size;
    size_t index = 0;
    if (key) index = (size_t)((char*)key - (char*)table->entries) / (entry_size);
    if (index > table->capacity){
        return NULL;
    }
    index++;
    while (index < table->capacity){
        RSSlotState state = (table->meta[index] & RSHASH_STATE_MASK) >> 6;
        if(state == RSHASH_TAKEN) return (char*)table->entries + index * entry_size;
    }
    return NULL;
}

RSAPI void *RShash_getValue(RSHash *table, void *key){
    return (char*)key + table->key_size;
}

RSSlotState RShash_getSlotState(RSHash *table, size_t index){
    return (table->meta[index] & RSHASH_STATE_MASK) >> 6;
}

RSAPI size_t RShash_getSize(RSHash *table){
    return table->size;
}

RSAPI size_t RShash_getCapacity(RSHash *table){
    return table->capacity;
}