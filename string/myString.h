#ifndef MYSTRING
#define MYSTRING

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

//how much larger will the capacity be than the size of a string when memory
//is being allocated, either during the creation of strings or during resizing
#define SIZE_RESERVE 10

//length based string
typedef struct String{
    size_t size;
    size_t capacity;
    char *text;
}String;


//create a String with enough sapce for 10 elements
String String_create();
//create a String with a desired amount of elemnts
String String_createCapacity(size_t capacity);

/*
//create a String with a desired amount of elements and set it to be full
String String_createSize(size_t size);
*/

//create a String with a desired amount of elements and fill it up with a value
String String_createSizeSet(size_t size, char value);
//create a String and assign it a text
String String_createAssign(char *text);
//free the String. If the elements also use dynamic memory, they don't get freed -> memory leaks
void String_destroy(String *string);


//copy some text into the String
void String_assign(String *string, const char *text);
//add a character to the end of a String
void String_appendChar(String *string, const char value);
//append text to the end of a String
void String_append(String *string, const char *text);
/*
//get the last element of a String and return it in the "ret" variable. Removes the last element
void String_pop(String *string, char *ret);
*/
//insert a text anywhere inside the string, if inserting outside the used memory 
//(i.e. String has 10 elements and we are inserting to position 15) nothing is done
void String_insert(String *string, const size_t index, const char *text);
//rewrite n characters of the string with the provided text where n is the length of the text
void String_set(String *string, const size_t index, const char *text);
//copy the element at an index to the "ret" variable
void String_get(String *string, const size_t index, char *ret);
//set size of the String to 0
void String_clear(String *string);
//change the capacity of a string, if current size is greater than te new capacity
//all of the trailing elements will be lost and string will be full after the resize
void String_resize(String *string, size_t capacity);


//creates a copy of an existing String. Returns the copy
String String_copyReturn(String *src);
//copies the String "src" into the String "dest". If String dest is not empty, it frees the memory.
void String_copy(String *dest, String *src);


//get the capacity of a String
long long int String_getCapacity(String *string);
//get the size (number of elements) of a String
long long int String_getSize(String *string);


//shifts all elements of a String from the position "index" to the right by "amount" 
void String_shiftRightFromBy(String *string, const size_t index, const size_t amount);
//shifts all elements of a String from the position "index" to the left by "amount"
void String_shiftLeftFromBy(String *string, const size_t index, const size_t amount);


//sort the String according to the supplied compare function
void String_print(const String *string);
void String_println(const String *string);

#endif