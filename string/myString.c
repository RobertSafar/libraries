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

MSAPI String String_createCapacity(ssize_t capacity){
    if(capacity < 0) return STRING_EMPTY();

    String string;
    string.size = 0;
    string.capacity = (size_t) capacity + 1;
    string.text = malloc(sizeof(char) * string.capacity);
    string.text[string.size] = '\0'; 
    return string;
}

/*
//create a String with a desired amount of elements and set it to be full
MSAPI String String_createSize(ssize_t size);
*/


MSAPI String String_createSizeSet(ssize_t size, char value){
    if(size < 0) return STRING_EMPTY();

    size_t sz = (size_t) size;
    String string;
    string.size = sz;
    string.capacity = sz + 1 + SIZE_RESERVE;
    string.text = malloc(sizeof(char) * string.capacity);
    memset(string.text, value, (sz * sizeof(char)));
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
    string->capacity = 0;
    string->size = 0;
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

MSAPI void String_insert(String *string, const ssize_t index, const char *text){
    if(!string || index < 0) return;

    size_t idx = (size_t) index;
    if(idx > string->size) return;
    else if(idx == string->size){
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
    memcpy(string->text + idx + len, string->text + idx, (sizeof(char) * string->size - idx));
    memcpy(string->text + idx, text, (sizeof(char) * len));
    string->size += len;
    string->text[string->size] = '\0';

    return;
}

MSAPI void String_set(String *string, const ssize_t index, const char *text){
    if(!string || !string->text) return;

    size_t idx = (size_t) index;
    if(idx >= string->size || index < 0){
        return;
    }
    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }

    size_t new_size = idx + len > string->size ? idx + len : string->size;

    if(new_size > string->capacity){
        string->capacity = new_size + 1 + SIZE_RESERVE;
        string->text = realloc(string->text, string->capacity);
    }
    memcpy(string->text + idx, text, (sizeof(char) * len));
    string->size = new_size;
    string->text[string->size] = '\0';

    return;
}

MSAPI int String_get(String *string, const ssize_t index, char *ret){
    if(!string || !string->text){
        *ret = '\0';
        return 0;
    }
    *ret = string->text[index];
    return 0;
}


MSAPI void String_clear(String *string){
    string->size = 0;
    return;
}

MSAPI void String_resize(String *string, ssize_t capacity){
    if(!string || capacity < 0) return;

    size_t cap = (size_t) capacity;
    if(!string->text){
        string->size = 0;
        string->capacity = cap;
        string->text = malloc(string->capacity + 1);
        string->text[0] = '\0';
    }
    else if(string->capacity == cap) return;
    else{
        string->size = string->size <= cap ? string->size : cap;
        string->capacity = cap;
        string->text = realloc(string->text, string->capacity + 1);
        string->text[string->size] = '\0';
    }
    return;
}



MSAPI String String_copyReturn(String *src){
    if(!src) return STRING_NULL();

    String string;
    string.size = src->size;
    string.capacity = src->capacity;
    string.text = malloc(src->capacity * sizeof(char));
    memcpy(string.text, src->text, sizeof(char) * string.size);
    string.text[string.size] = '\0';
    return string;
}

MSAPI void String_copy(String *dest, String *src){
    if(!dest || !src) return;
    // if(dest->text){
    //     free(dest->text);
    // }
    dest->size = src->size;
    dest->capacity = src->capacity;
    dest->text = malloc(src->capacity * sizeof(char));
    memcpy(dest->text, src->text, sizeof(char) * dest->size);
    dest->text[dest->size] = '\0';
    return;
}



MSAPI size_t String_getCapacity(String *string){
    if(string) return string->capacity;
    return 0;
}

MSAPI size_t String_getSize(String *string){
    if(string) return string->size;
    return 0;
}

MSAPI char *String_getTextPointer(String *string){
    if(string) return string->text;
    return NULL;
}



MSAPI void String_shiftRightFromBy(String *string, const ssize_t index, const ssize_t amount){
    if(!string || amount == 0 || !string->size) return;

    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= string->size || index < 0) {
        return;
    }
    else if(string->size + amt > string->capacity){
        string->capacity = string->size + amt + SIZE_RESERVE;
        string->text = realloc(string->text, string->capacity);
    }
    string->size += amt;
    memmove(string->text + idx + amt, string->text + idx, sizeof(char) * (string->size - idx));
    memset(string->text + idx, ' ', amt);
    string->text[string->size] = '\0';
    return;
}

MSAPI void String_shiftLeftFromBy(String *string, const ssize_t index, const ssize_t amount){
    if(!string || amount <= 0 || !string->size) return;

    size_t amt = (size_t) amount;
    size_t idx = (size_t) index;
    if(idx >= string->size || index < 0) {
        return;
    }
    size_t new_index = idx >= string->size ? string->size - 1 : idx;
    size_t copy_len = new_index >= amt ? new_index - amt + 1 : 0;
    memmove(string->text, string->text + amt, sizeof(char) * copy_len);
    memset(string->text + copy_len, ' ', new_index + 1 - copy_len);

    if(new_index == (string->size - 1)) string->size = string->size > amt ? string->size - amt : 0;
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