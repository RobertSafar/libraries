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
    if(!string || !string->text) return;

    free(string->text);
    string->text = NULL;
    string->capacity = 0;
    string->size = 0;
    return;
}



MSAPI void String_assign(String *string, const char *text){
    if(!string || !text) return;

    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }
    if(len >= string->capacity){
        string->capacity = len + 1 + SIZE_RESERVE;
        free(string->text);
        string->text = malloc(sizeof(char) * string->capacity);
    }
    memcpy(string->text, text, (sizeof(char) * len));
    string->size = len;
    string->text[string->size] = '\0';
    return;
}

MSAPI void String_assignLength(String *string, const char *text, size_t length){
    if(!string || !text) return;

    for(size_t i = 0; i < length; i++){
        if(text[i] == '\0'){
            length = i;
            break;
        }
    }
    if(length >= string->capacity){
        string->capacity = length + 1 + SIZE_RESERVE;
        free(string->text);
        string->text = malloc(sizeof(char) * string->capacity);
    }
    memcpy(string->text, text, (sizeof(char) * length));
    string->size = length;
    string->text[string->size] = '\0';
    return;
}

MSAPI void String_assignString(String *string, String *src){
    if(!src || !string) return;

    if(src->size >= string->capacity){
        string->capacity = src->size + 1 + SIZE_RESERVE;
        free(string->text);
        string->text = malloc(sizeof(char) * string->capacity);
    }
    memcpy(string->text, src->text, (sizeof(char) * src->size));
    string->size = src->size;
    string->text[string->size] = '\0';
    return;
}

MSAPI void String_appendChar(String *string, const char value){
    if(!string) return;
    if(string->size + 1 == string->capacity){
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
    
    if(string->size + len >= string->capacity){
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
    
    if(string->size + len >= string->capacity){
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

MSAPI inline char String_getUnsafe(String *string, const size_t index){
    return string->text[index];
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

MSAPI char *String_getTextPointerOffset(String *string, ssize_t offset){
    if(offset >= 0 && (size_t)offset < string->size && string) return string->text + (size_t)offset;
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



MSAPI bool String_isEqual(String *str1, String *str2){
    if(str1->size != str2->size) return false;

    for(size_t i = 0; i < str1->size; i++){
        if(str1->text[i] != str2->text[i]) return false;
    }
    return true;
}

long long int String_chr(String *str, char character){
    if(str->size <= 0) return -1;

    for(size_t i = 0; i < str->size; i++){
        if(str->text[i] == character) return (long long int)i;
    }
    return -1;
}

void String_stripWhiteSpaces(String *string){
    if(!string || !string->size) return;

    //first non white space character
    size_t first_char = 0;
    //last non white space character
    size_t last_char = 0;
    //are there any non white space character
    bool found_char = false;
    for(size_t i = 0; i < string->size; i++){
        switch (string->text[i])
        {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            continue;;
        default:
            found_char = true;
            first_char = i;
            last_char = i;
            goto character_found;
            break;
        }
    }

    if(!found_char){
        string->size = 0;
        string->text = '\0';
        return;
    }

    character_found:
    for(size_t i = first_char + 1; i < string->size; i++){
        switch (string->text[i])
        {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            continue;;
        default:
            last_char = i;
            break;
        }
    }

    string->size = last_char + 1;
    string->text[last_char + 1] = '\0';
    String_shiftLeftFromBy(string, (ssize_t)last_char, (ssize_t)first_char);
    return;
}

void String_removeCharacter(String *string, char character){
    if(!string || !string->size) return;

    size_t write_index = 0;
    size_t read_index = 0;
    // for(size_t i = 0; i < string->size; i++){
    //     if(string->text[i] == character){
    //         write_index = i;
    //         read_index = i + 1;
    //         break;
    //     }
    // }


    for(size_t i = write_index; i < string->size; i++){
        if(string->text[i] != character){
            string->text[write_index] = string->text[i];
            write_index++;
            continue;
        }
    }
    string->size = write_index;
    string->text[write_index] = '\0';
}

void String_removeCharacterSet(String *string, String *characters){
    if(!string || !characters || !string->size || !characters->size) return;

    bool look_up[256] = {false};
    for(size_t i = 0; i < characters->size; i++){
        unsigned char c = characters->text[i];
        look_up[c] = true;
    }

    size_t write_index = 0;
    for(size_t i = write_index; i < string->size; i++){
        unsigned char c = string->text[i];
        if(!look_up[c]){
            string->text[write_index] = string->text[i];
            write_index++;
            continue;
        }
    }
    string->size = write_index;
    string->text[write_index] = '\0';
}

bool String_endsWith(String *string, const char *suffix){
    if(!string || !string->size || !suffix) return false;

    size_t len = 0;
    while (len <= string->size && suffix[len] != '\0'){
        len++;
    }

    if(len > string->size) return false;

    size_t indexStr = string->size - 1;
    size_t indexSuf = len - 1;
    for(size_t i = 0; i < len; i++){
        if(string->text[indexStr] != suffix[indexSuf]) return false;
        indexStr--;
        indexSuf--;
    }
    return true;
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