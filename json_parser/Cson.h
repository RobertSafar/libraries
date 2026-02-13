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
    JSON_NULL,
    JSON_ARRAY,
    JSON_OBJECT
}JsonType;


typedef struct JsonKeyValuePair JsonKeyValuePair;
typedef struct JsonParserValue JsonParserValue;

typedef struct JsonParserValue{
    JsonType type;
    union {
        int number;
        double decimal;
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



void Cson_free(JsonParserValue *node);
JsonParserValue Cson_getObject(String *data, size_t *start);
JsonParserValue Cson_getArray(String *data, size_t *start);
JsonParserValue Cson_getString(String *data, size_t *start);
JsonParserValue Cson_getBoolean(String *data, size_t *start);
JsonParserValue Cson_getNull(String *data, size_t *start);
JsonParserValue Cson_getNumeric(String *data, size_t *start);
JsonParserValue Cson_loadData(String *data);

#endif