// #ifndef MYSTRING_C
// #define MYSTRING_C

#include "myString.h"


#ifdef _WIN32
    #ifdef BUILDING_DLL
        #define MSAPI __declspec(dllexport)
    #elif defined(USING_DLL)
        #define MSAPI __declspec(dllimport)
    #else
        #define MSAPI
    #endif
#else
    #define MSAPI   /* nothing */
#endif



MSAPI String String_create(){
    String string;
    string.size = 0;
    string.capacity = 10 + 1;
    string.text = malloc(sizeof(char) * string.capacity);
    string.text[string.size] = '\0'; 
    return string;
}

MSAPI String String_createCapacity(size_t capacity){
    String string;
    string.size = 0;
    string.capacity = capacity + 1;
    string.text = malloc(sizeof(char) * string.capacity);
    string.text[string.size] = '\0'; 
    return string;
}

/*
//create a String with a desired amount of elements and set it to be full
MSAPI String String_createSize(size_t size);
*/


MSAPI String String_createSizeSet(size_t size, char value){
    String string;
    string.size = size;
    string.capacity = size + 1 + SIZE_RESERVE;
    string.text = malloc(sizeof(char) * string.capacity);
    memset(string.text, value, (size * sizeof(char)));
    string.text[string.size] = '\0'; 
    return string;
}

MSAPI String String_createAssign(char *text){
    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }
    String string;
    string.size = len;
    string.capacity = len + 1 + SIZE_RESERVE;
    string.text = malloc(sizeof(char) * string.capacity);
    memcpy(string.text, text, (sizeof(char) * len));
    string.text[string.size] = '\0'; 
    return string;
}

MSAPI void String_destroy(String *string){
    free(string->text);
    string->text = NULL;
    return;
}



MSAPI void String_assign(String *string, const char *text){
    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }
    if(len > string->capacity){
        string->capacity = len + 1 + SIZE_RESERVE;
        free(string->text);
        string->text = malloc(sizeof(char) * string->capacity);
    }
    memcpy(string->text, text, (sizeof(char) * len));
    string->size = len;
    string->text[string->size] = '\0';
    return;
}

MSAPI void String_appendChar(String *string, const char value){
    if(!string) return;
    if(string->size == string->capacity){
        string->capacity *= 2;
        string->text = realloc(string->text, string->capacity);
    }
    string->text[string->size] = value;
    string->size++;
    string->text[string->size] = '\0';
    return;
}

MSAPI void String_append(String *string, const char *text){
    if(!string) return;

    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }
    
    if(string->size + len > string->capacity){
        string->capacity = string->size + len + 1 + SIZE_RESERVE;
        string->text = realloc(string->text, string->capacity);
    }
    memcpy(string->text + string->size, text, (sizeof(char) * len));
    string->size += len;
    string->text[string->size] = '\0';
    return;
}
/*
//get the last element of a String and return it in the "ret" variable. Removes the last element
MSAPI void String_pop(String *string, char *ret);
*/

MSAPI void String_insert(String *string, const size_t index, const char *text){
    if(!string) return;
    else if(index > string->size) return;
    else if(index == string->size){
        String_append(string, text);
        return;
    }

    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }
    
    if(string->size + len > string->capacity){
        string->capacity = string->size + len + 1 + SIZE_RESERVE;
        string->text = realloc(string->text, string->capacity);
    }
    memcpy(string->text + index + len, string->text + index, (sizeof(char) * string->size - index));
    memcpy(string->text + index, text, (sizeof(char) * len));
    string->size += len;
    string->text[string->size] = '\0';

    return;
}

MSAPI void String_set(String *string, const size_t index, const char *text){
    if(!string) return;

    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }

    size_t new_size = index + len > string->size ? index + len : string->size;

    if(new_size > string->capacity){
        string->capacity = new_size + 1 + SIZE_RESERVE;
        string->text = realloc(string->text, string->capacity);
    }
    memcpy(string->text + index, text, (sizeof(char) * len));
    string->size = new_size;
    string->text[string->size] = '\0';

    return;
}
/*
//copy the element at an index to the "ret" variable
MSAPI void String_get(String *string, const size_t index, char *ret);
*/

MSAPI void String_clear(String *string){
    string->size = 0;
    return;
}



MSAPI String String_copyReturn(String *src){
    String string;
    string.size = src->size;
    string.capacity = src->capacity;
    string.text = malloc(src->capacity * sizeof(char));
    memcpy(string.text, src->text, sizeof(char) * string.size);
    string.text[string.size] = '\0';
    return string;
}

MSAPI void String_copy(String *dest, String *src){
    if(dest->text){
        free(dest->text);
    }
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->text = malloc(src->capacity * sizeof(char));
    memcpy(dest->text, src->text, sizeof(char) * dest->size);
    dest->text[dest->size] = '\0';
    return;
}



MSAPI long long int String_getCapacity(String *string){
    if(string) return string->capacity;
    return 0;
}

MSAPI long long int String_getSize(String *string){
    if(string) return string->size;
    return 0;
}



MSAPI void String_shiftRightFromBy(String *string, const size_t index, const size_t amount){
    if(!string || index >= string->size || amount == 0 || index < 0) return;
    else if(string->size + amount > string->capacity){
        string->capacity = string->size + amount + SIZE_RESERVE;
        string->text = realloc(string->text, string->capacity);
    }
    string->size += amount;
    memmove(string->text + index + amount, string->text + index, sizeof(char) * (string->size - index));
    memset(string->text + index, ' ', amount);
    string->text[string->size] = '\0';
    return;
}

MSAPI void String_shiftLeftFromBy(String *string, const size_t index, const size_t amount){
    if(!string || index < 0 || amount == 0) return;
    size_t new_index = index >= string->size ? string->size - 1 : index;
    size_t copy_len = new_index >= amount ? new_index - amount + 1 : 0;
    memmove(string->text, string->text + amount, sizeof(char) * copy_len);
    memset(string->text + copy_len, ' ', new_index + 1 - copy_len);

    if(new_index == (string->size - 1)) string->size = string->size > amount ? string->size - amount : 0;
    string->text[string->size] = '\0';

    return;
}



MSAPI void String_print(const String *string){
    for(int i = 0; i < string->size; i++){
        char character = *(string->text + i);
        putchar(character);
    }
    return;
}

MSAPI void String_println(const String *string){
    for(int i = 0; i < string->size; i++){
        char character = *(string->text + i);
        putchar(character);
    }
    putchar('\n');
    return;
}

// #endif