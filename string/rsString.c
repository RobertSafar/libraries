#include "rsString.h"


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



RSAPI RSStr RSstr_create(){
    RSStr string;
    string.size = 0;
    string.capacity = 10 + 1;
    string.text = malloc(sizeof(char) * string.capacity);
    string.text[string.size] = '\0'; 
    return string;
}

RSAPI RSStr RSstr_createCapacity(ssize_t capacity){
    if(capacity < 0) return STRING_EMPTY();

    RSStr string;
    string.size = 0;
    string.capacity = (size_t) capacity + 1;
    string.text = malloc(sizeof(char) * string.capacity);
    string.text[string.size] = '\0'; 
    return string;
}




RSAPI RSStr RSstr_createSizeSet(ssize_t size, char value){
    if(size < 0) return STRING_EMPTY();

    size_t sz = (size_t) size;
    RSStr string;
    string.size = sz;
    string.capacity = sz + 1 + SIZE_RESERVE;
    string.text = malloc(sizeof(char) * string.capacity);
    memset(string.text, value, (sz * sizeof(char)));
    string.text[string.size] = '\0'; 
    return string;
}

RSAPI RSStr RSstr_createAssign(char *text){
    size_t len = 0;
    while(*(text + len) != '\0'){
        len++;
    }
    RSStr string;
    string.size = len;
    string.capacity = len + 1 + SIZE_RESERVE;
    string.text = malloc(sizeof(char) * string.capacity);
    memcpy(string.text, text, (sizeof(char) * len));
    string.text[string.size] = '\0'; 
    return string;
}

RSAPI void RSstr_destroy(RSStr *string){
    if(!string || !string->text) return;

    free(string->text);
    string->text = NULL;
    string->capacity = 0;
    string->size = 0;
    return;
}

RSAPI char *RSstr_detach(RSStr *string){
    char *retval = string->text;
    string->size = 0;
    string->capacity = 0;
    string->text = NULL;
    return retval;
}


RSAPI void RSstr_assign(RSStr *string, const char *text){
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

RSAPI void RSstr_assignLength(RSStr *string, const char *text, size_t length){
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

RSAPI void RSstr_assignString(RSStr *string, RSStr *src){
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

RSAPI void RSstr_appendChar(RSStr *string, const char value){
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

RSAPI void RSstr_append(RSStr *string, const char *text){
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

RSAPI void RSstr_appendLength(RSStr *string, const char *text, ssize_t length){
    if(!string || length <= 0) return;

    
    if(string->size + length >= string->capacity){
        string->capacity = string->size + length + 1 + SIZE_RESERVE;
        string->text = realloc(string->text, string->capacity);
    }
    memcpy(string->text + string->size, text, (sizeof(char) * length));
    string->size += length;
    string->text[string->size] = '\0';
    return;
}


RSAPI void RSstr_insert(RSStr *string, const ssize_t index, const char *text){
    if(!string || index < 0) return;

    size_t idx = (size_t) index;
    if(idx > string->size) return;
    else if(idx == string->size){
        RSstr_append(string, text);
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

RSAPI void RSstr_set(RSStr *string, const ssize_t index, const char *text){
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

RSAPI int RSstr_get(RSStr *string, const ssize_t index, char *ret){
    if(!string || !string->text){
        *ret = '\0';
        return 0;
    }
    *ret = string->text[index];
    return 0;
}

RSAPI char RSstr_getUnsafe(RSStr *string, const ssize_t index){
    return string->text[index];
}


RSAPI void RSstr_clear(RSStr *string){
    string->size = 0;
    return;
}

RSAPI void RSstr_resize(RSStr *string, ssize_t capacity){
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



RSAPI RSStr RSstr_copyReturn(RSStr *src){
    if(!src) return STRING_NULL();

    RSStr string;
    string.size = src->size;
    string.capacity = src->capacity;
    if(src->capacity){
        string.text = malloc(src->capacity * sizeof(char));
    }
    else{
        string.text = NULL;
    }
    memcpy(string.text, src->text, sizeof(char) * string.size);
    string.text[string.size] = '\0';
    return string;
}

RSAPI void RSstr_copy(RSStr *dest, const RSStr *src){
    if(!dest || !src) return;

    dest->size = src->size;
    dest->capacity = src->capacity;
    if(src->capacity){
        dest->text = malloc(src->capacity * sizeof(char));
    }
    else{
        dest->text = NULL;
    }
    memcpy(dest->text, src->text, sizeof(char) * dest->size);
    dest->text[dest->size] = '\0';
    return;
}

RSAPI void RSstr_copyShallow(RSStr *dest, const RSStr *src){
    if(!dest || !src) return;
    memcpy(dest, src, sizeof(RSStr));
    return;
}



RSAPI size_t RSstr_getCapacity(RSStr *string){
    if(string) return string->capacity;
    return 0;
}

RSAPI size_t RSstr_getSize(RSStr *string){
    if(string) return string->size;
    return 0;
}

RSAPI char *RSstr_getTextPointer(RSStr *string){
    if(string) return string->text;
    return NULL;
}

RSAPI char *RSstr_getTextPointerOffset(RSStr *string, ssize_t offset){
    if(offset >= 0 && (size_t)offset < string->size && string) return string->text + (size_t)offset;
    return NULL;
}



RSAPI void RSstr_shiftRightFromBy(RSStr *string, const ssize_t index, const ssize_t amount){
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

RSAPI void RSstr_shiftLeftFromBy(RSStr *string, const ssize_t index, const ssize_t amount){
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



RSAPI bool RSstr_isEqual(RSStr *str1, RSStr *str2){
    if(str1->size != str2->size) return false;

    for(size_t i = 0; i < str1->size; i++){
        if(str1->text[i] != str2->text[i]) return false;
    }
    return true;
}

long long int RSstr_chr(RSStr *str, char character){
    if(str->size <= 0) return -1;

    for(size_t i = 0; i < str->size; i++){
        if(str->text[i] == character) return (long long int)i;
    }
    return -1;
}

void RSstr_stripWhiteSpaces(RSStr *string){
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
        string->text[0] = '\0';
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
    RSstr_shiftLeftFromBy(string, (ssize_t)last_char, (ssize_t)first_char);
    return;
}

void RSstr_removeCharacter(RSStr *string, char character){
    if(!string || !string->size) return;

    size_t write_index = 0;
    size_t read_index = 0;


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

void RSstr_removeCharacterSet(RSStr *string, RSStr *characters){
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

void RSstr_removeCharacterSetC(RSStr *string, char *characters){
    if(!string || !characters || !string->size) return;

    bool look_up[256] = {false};
    size_t size = strlen(characters);
    for(size_t i = 0; i < size; i++){
        unsigned char c = characters[i];
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

bool RSstr_endsWith(RSStr *string, const char *suffix){
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



RSAPI void RSstr_print(const RSStr *string){
    for(int i = 0; i < string->size; i++){
        char character = *(string->text + i);
        putchar(character);
    }
    return;
}

RSAPI void RSstr_println(const RSStr *string){
    for(int i = 0; i < string->size; i++){
        char character = *(string->text + i);
        putchar(character);
    }
    putchar('\n');
    return;
}