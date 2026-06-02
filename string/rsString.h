#ifndef RS_STRING_H
#define RS_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>

//how much larger will the capacity be than the size of a string when memory
//is being allocated, either during the creation of strings or during resizing
#define SIZE_RESERVE 10
#ifndef SSIZE_T
#define SSIZE_T
    #if defined (_WIN32) || defined(_WIN64)
        typedef long long int ssize_t;
    #endif
#endif


//length based string
typedef struct RSStr{
    size_t size;
    size_t capacity;
    char *text;
}RSStr;

//string where all elements are set to 0
#define STRING_NULL() (RSStr){0, 0, NULL}
//string where all elements are set to 0
#define STRING_EMPTY() (RSStr){0, 0, NULL}

//create a RSStr with enough space for 10 elements
RSStr RSstr_create();
//create a RSStr with a desired amount of elemnts
RSStr RSstr_createCapacity(ssize_t capacity);


//create a RSStr with a desired amount of elements and fill it up with a value
RSStr RSstr_createSizeSet(ssize_t size, char value);
//create a RSStr and assign it a text
RSStr RSstr_createAssign(char *text);
//free the RSStr. If the elements also use dynamic memory, they don't get freed -> memory leaks
void RSstr_destroy(RSStr *string);
//destroy the string without freeing the dynamic memory. Pointer to this memory is returnd.
char *RSstr_detach(RSStr *string);


//copy some text into the RSStr
void RSstr_assign(RSStr *string, const char *text);
//copy some text of length "length" from input "text"
void RSstr_assignLength(RSStr *string, const char *text, size_t length);
//copy the text from src string into the RSStr
void RSstr_assignString(RSStr *string, RSStr *src);
//add a character to the end of a RSStr
void RSstr_appendChar(RSStr *string, const char value);
//append text to the end of a RSStr
void RSstr_append(RSStr *string, const char *text);
//append text to the end of a RSStr. The number of character copied is specified in "length" variable
//this allows us to copy part of a text if we only want a part of it.
//It also allows us to copy binary data, which may contain several '\0' characters, into a RSStr
void RSstr_appendLength(RSStr *string, const char *text, ssize_t length);

//insert a text anywhere inside the string, if inserting outside the used memory 
//(i.e. RSStr has 10 elements and we are inserting to position 15) nothing is done
void RSstr_insert(RSStr *string, const ssize_t index, const char *text);
//rewrite n characters of the string with the provided text where n is the length of the text
void RSstr_set(RSStr *string, const ssize_t index, const char *text);
//copy the element at an index to the "ret" variable
int RSstr_get(RSStr *string, const ssize_t index, char *ret);
//return the character at the index "index" without checking if "string" exists or is NULL,
//and without checking if the index is valid
char RSstr_getUnsafe(RSStr *string, const ssize_t index);
//set size of the RSStr to 0
void RSstr_clear(RSStr *string);
//change the capacity of a string, if current size is greater than te new capacity
//all of the trailing elements will be lost and string will be full after the resize
void RSstr_resize(RSStr *string, ssize_t capacity);


//creates a copy of an existing RSStr. Returns the copy
RSStr RSstr_copyReturn(RSStr *src);
//copies the RSStr "src" into the RSStr "dest". If RSStr dest is not empty, the pointer to this memory is lost.
//dest MUST be freed before calling to copy function
void RSstr_copy(RSStr *dest, const RSStr *src);
//copy the "src" RSStr into "dest" RSStr verbatim. The pointer to the text is also copied which means that both Strings point to the same piece of memory. BE CAREFUL! 
void RSstr_copyShallow(RSStr *dest, const RSStr *src);


//get the capacity of a RSStr
size_t RSstr_getCapacity(RSStr *string);
//get the size (number of elements) of a RSStr
size_t RSstr_getSize(RSStr *string);
//return the pointer to the string
char *RSstr_getTextPointer(RSStr *string);
//return the pointer to the string starting at index "offset"
char *RSstr_getTextPointerOffset(RSStr *string, ssize_t offset);


//shifts all elements of a RSStr from the position "index" to the right by "amount" 
void RSstr_shiftRightFromBy(RSStr *string, const ssize_t index, const ssize_t amount);
//shifts all elements of a RSStr from the position "index" to the left by "amount"
void RSstr_shiftLeftFromBy(RSStr *string, const ssize_t index, const ssize_t amount);


//compare 2 strings
bool RSstr_isEqual(RSStr *str1, RSStr *str2);
//return the index of the first occurance of "character"
long long int RSstr_chr(RSStr *str1, char character);
//remove white spaces from the beginning and end of a string
void RSstr_stripWhiteSpaces(RSStr *string);
//remove all letters that match "character" from "string"
void RSstr_removeCharacter(RSStr *string, char character);
//remove all letters that can be found in the "characters" RSStr from "string"
void RSstr_removeCharacterSet(RSStr *string, RSStr *characters);
//remove all letters that can be found in the "characters" C string from "string"
void RSstr_removeCharacterSetC(RSStr *string, char *characters);
//check if a string ends with a certain suffix
bool RSstr_endsWith(RSStr *string, const char *suffix);


//print the string to STDOUT
void RSstr_print(const RSStr *string);
//print the string to STDOUT and follow it with a newlinew character
void RSstr_println(const RSStr *string);

#endif