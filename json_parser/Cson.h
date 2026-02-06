#ifndef CSON
#define CSON

#include <stdbool.h>

#include "../string/myString.h"
#include "../vector/vector.h"


typedef enum{
    JSON_ERROR,
    JSON_STRING,
    JSON_INT,
    JSON_DOUBLE,
    JSON_BOOLEAN,
    JSON_ARRAY,
    JSON_OBJECT
}JsonType;


typedef struct JsonKeyValuePair JsonKeyValuePair;
typedef struct JsonParserValue JsonParserValue;

typedef struct JsonParserValue{
    JsonType type;
    union {
        int number;
        bool boolean;
        String string;
        CVector array;
        CVector object;
    }val;
}JsonParserValue;

typedef struct JsonKeyValuePair{
    String key;
    JsonParserValue value;
}JsonKeyValuePair;

typedef struct JsonParser{
    int a;
}JsonParser;

typedef void (*jsonParseFunction)();




JsonParserValue Cson_loadData(String *data);

#endif