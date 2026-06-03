# Disclaimer
This is a Hash Table implementation in C made for learning and personal use purposes. It may contain bugs, and may lack some functions I haven't thought about implementing yet.

# Documentation

This Hash Table is capable of storing any type of data. It does this by storing the key and value sizes and functions to properly manage them. 

Attributes of the Hash Table:

- size_t **size**
- size_t **capacity**
- unsigned int **key_size**
- unsigned int **value_size**

- uint8_t *meta
- void *entries

- rsHashCompare **compare**
- rsHashFunction **hash**
- rsHashCopy **copy**
- rsHashDelete **delete**

**size**, **capacity**, **meta** and **entries** are internal attributes and must not be modified by your code. They are updated automatically by the provided API functions.

**key_size** and **value_size** are the sizes of the key and value. They are used to calculate how much memory is to be allocated and during indexing. They are set during the creation and must not be changed during the lifetime of the Hash Table.

**compare** and **hash** are function pointers the Hash Table uses to find a given element or a free slot. They **MUST NOT** be **NULL**.


**copy** and **delete** are helper function that help the Hash Table manage memory. They **can be NULL**, in which case the key and value are treated as simple data types that don't manage dynamic memory.

In case that the **copy** function is **NULL**, the key and value are copied into the Hash Table by the use of *memcpy* function. If the key or the value holds a pointer to dynamic memory, this pointer will get copied inside of the Hash Table and both the Table entry and the original variables will point to the same piece of memory (changing the memory address pointed to by the input will result in the change of data pointed to by data stored in the Table).

In case that the **delete** function is **NULL**, all of the entries together with the **metadata** will be deleted as a single block of memory. If either the keys or values hold pointers to dynamic memory, these will be lost.

## Hash Table creation
There are 2 functions that are meant to be used to create a Hash Table:
### RShash_create:
```c
RSHash RShash_create(
    unsigned int key_size,
    unsigned int value_size,
    rsHashCompare compare,
    rsHashFunction hash,
    rsHashCopy copy,
    rsHashDelete delete
)
```
creates a Hash table of capacity 10.
### RShash_createCapacity:
```c
RSHash RShash_createCapacity(
    unsigned int key_size,
    unsigned int value_size, 
    rsHashCompare compare, 
    rsHashFunction hash, 
    size_t capacity, 
    rsHashCopy copy, 
    rsHashDelete delete
)
```
creates a Hash Table with the desired capacity.

**Parameters**

| Type           | Parameter    | Description                              |
|:---------------|:-------------|:-----------------------------------------|
| `unsigned int` | `key_size`   | Size of the key in bytes                 |
| `unsigned int` | `value_size` | Size of the value in bytes               |
| `rsHashCompare`| `compare`    | Function used to compare keys            |
| `rsHashFunction`| `hash`      | Hash function for computing slot index   |
| `size_t`       | `capacity`   | How many entries do we want to allocate space for|
| `rsHashCopy`   | `copy`       | Copy helper, or `NULL` for plain memcpy  |
| `rsHashDelete` | `delete`     | Delete helper, or `NULL` for block free  |

**Returns** — newly created Hash Table, **Type** RSHash.

### Examples
creating a Hash Table where both the key and value are of type "int"
```c
RSHash table = RShash_create(sizeof(int), sizeof(int), compare, hash, NULL, NULL);
```
creating a Hash table where the key is of type "char*" and value is of type "int"
```c
RSHash table = RShash_create(sizeof(char*), sizeof(int), compare, hash, copy, delete);
```
creating a Hash table where the key is of type "RSStr" and value is of type "RSManagedVec"
```c
RSHash table = RShash_create(sizeof(RSStr), sizeof(RSManagedVec), compare, hash, copy, create);
```

**NOTE** the functions (compare, hash, copy and delete) must be written by you. They are documented later in this README.

## Hash Table manipulation

There are 3 functions that allow us to interract with the data stored inside of the Table.
### RShash_search:
```c
void *RShash_search(
    RSHash *table, 
    void *key
)
```
**RShash_search** is used to get a pointer to the value associated with the key passed in as input. 

### RSHash_insert:
```c
void *RShash_insert(
    RSHash *table, 
    void *key, 
    void *value
)
```
**RShash_insert** is used to insert a key value pair into the Table. If the key already exists, it returns without performing any change. It also returns the pointer to the value associated with the key passed in as input. 

### RSHash_delete:
```c
void RShash_delete(
    RSHash *table, 
    void *key
)
```
**RShash_delete** is used to delete a key value pair from the Table. It doesn't return anything.

### Examples
when both the key and value are of type int

```c
int key = 1;
int value = 1;
RShash_insert(&table, &key, &value);
RShash_search(&table, &key);
RShash_delete(&table, &key);
```

### Updating values
Updating values is done by directly modifying the data pointed to by the void pointer returned by either the *RShash_search* or *RShash_insert* functions. If you want to update a value associated with an existing key but don't do anything if the key doesn't exist you can use the **RShash_search** function to get the pointer to the value, if the key doesn't exist, return value is **NULL**. If you want to update a value associated with an existing key, and create a key value pair if the key doesn't exist, you can use the **RShash_insert** function to get the pointer to the value. If such doesn't exist yet, it will be inserted together with the value, pointer to which is then returned and can be modified by you.

### Examples
when both key and value are of type int
```c
int *val = (int*)RShash_search(&table, &key);
if(val) *val = 5;
```
This will return the pointer to the value associated with the key passed as input, and the value can be directly modified via the pointer. If the key doesn't exist yet, it returns NULL and thus it requires the check to see if the returned value isn't NULL before changing the value.
```c
int *val = (int*)RShash_insert(&table, &key, &value);
*val = 5;
```
This will always return the pointer to a Hash Table associated with the input key. If the key doesn't exist yet, it insets it and the value passed in as argument. This value can then be changed via the returned pointer.

## Getter functions for structure attributes

These functions are here to help you retrieve information about the Hash Table as well as help you iterate through the Hash Table as there is no iterator.

### RShash_getkey:
```c
void *RShash_getKey(
    RSHash *table, 
    size_t index
)
```
Retrieves the pointer to the key stored at "index". This key should not be manually modified via the pointer as that can break the function of the Hash Table. 

### RShash_getNextKey:
```c
void *RShash_getNextKey(
    RSHash *table, 
    void *key
)
```
Retrieves the pointer to the next key after the input key. If the input key is NULL, it retrieves the first key of the Hash Table. Can be used for iteration over the key value pairs of the Table. The keys returned by this fucntion should not be manually modified as that can break the function of the Hash Table.
### Example 
```c
void *key = RShash_getNextKey(&table, NULL);
while(key != NULL){
    int *val = (int*)RShash_getValue(&table, key);
    // do something with key and val
    key = RShash_getNextKey(&table, key);
}
```
We get the first existing key by calling the **RShash_getNextKey** with *NULL*. Then we keep calling this function until the return value becomes NULL.

### RShash_getValue:
```c
void *RShash_getValue(
    RSHash *table, 
    void *key
)
```
Retrieves the pointer to value associated with the key passed as an input. Meant to be used after **RShash_getNextKey** and **RShash_getKey**. Modifying the value is allowed, the Hash Table will continue working properly, but be careful not to cause any memory leaks, or overwritting data you don't want overwritten.

### RShash_getSlotState:
```c
RSSlotState RShash_getSlotState(
    RSHash *table, 
    size_t index
)
```
Returns the state of the slot at index "index". Every slot inside of the Hash Table is either.
- Empty (RSHAH_EMPTY) - slot has not been written to
- Taken (RSHASH_TAKEN) - slot has been written to and contains data
- DELETED (RSHASH_DELETED) - slot has been written to, but the data has been deleted
This function must be used if you want to iterate over all slots by index:
```c
for(size_t i = 0; i < RShash_getCapacity(&table); i++){
    RSSlotState state = RShash_getSlotState(&table, i);
    if(state == RSHASH_TAKEN){...}
}
```

### RShash_getSize:
```c
size_t RShash_getSize(
    RSHash *table
)
```
Returns the size (number of elements) of a Table.

### RShash_getCapacity:
```c
size_t RShash_getCapacity(
    RSHash *table
)
```
Returns the capacity (number of elements that can be stored) of a Table.


## Helper Functions

Functions stored as attributes by the Hash Table that are necessary for the appropriate function of the Hash Table. There are 4 functions here, 2 necessary and 2 optional.

**Necessary functions:**

### rsHashFunction:
```c
typedef size_t (*rsHashFunction)(void *key);
```
This function determines how is the index for the key going to be calculated. This function must not be NULL or the Table will not work.
### rsHashCompare:
```c
typedef bool (*rsHashCompare)(void *key1, void *key2);
```
This function determines how 2 keys are going to be compared. It is suppose to define when 2 instances of data type are equal to each other. This function must not be NULL or the Table will not work.

### Examples
when both the key and value are of type int, we can for example use the numerical value of the key itself as the index
```c
size_t hashFunction(void *key){
    int index = *(int*)key;
    return index;
}
```
the table then handles turning this index into a valid index based on it's capacity.
```c
bool compareFunction(void *key1, void *key2){
    int k1 = *(int*)key1;
    int k2 = *(int*)key2;
    if (k1 == k2) return true;
    return false;
}
```


**Optional functions**

### rsHashDelete:
```c
typedef void (*rsHashDelete)(void *key, void *value);
```
This function manages how to release resources held by the key value pair. It manages both the key and value, and so your implementation should define how both the key and the value are to be freed.  
If either the key or the value are a data type that doesn't hold a pointer to dynamic memory you can omit this in your delete function. If neither the key nor the value hold pointers to dynamic memory, rsHashDelete can be passed as NULL.
### rsHashCopy
```c
typedef void (*rsHashCopy)(void *slot_key, void *slot_value, void *key, void *value);
```
This function determines how is the key value pair going to be copied into the Hash Table. Unlike with the delete function, your implementation of this function must define how both the key and value are going to be copied. If either the key or the value don't hold a pointer to dynamic memory, you can use **memcpy** to copy this value. If neither the key nor the value hold pointers to dynamic memory rsHashCopy can be passed as NULL.

### Examples
when the key is of type **char\*** and value is of type **int**
```c
void copyFunction(void *slot_key, void *slot_value, void *key, void *value){
    
    char sk = (char**)slot_key;
    char *k = *(char**)key;
    *sk = (char*)malloc(strlen(k) * sizeof(char) + 1);
    strcpy(*sk, k);

    memcpy(slot_value, value, sizeof(int));
}
```
```c
void deleteFunction(void *key, void *value){
    
    char sk = (char**)slot_key;
    free(*sk);
}
```
