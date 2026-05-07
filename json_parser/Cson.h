#ifndef CSON
#define CSON

#include <stdbool.h>

#include "../string/rsString.h"
#include "../vector/rsVector.h"


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
        RSStr string;
        RSManagedVec array;
        RSManagedVec object;
    }val;
}JsonParserValue;

typedef struct JsonKeyValuePair{
    RSStr key;
    JsonParserValue value;
}JsonKeyValuePair;


void Cson_printNode(JsonParserValue *node);

void Cson_printKeyValtype(JsonKeyValuePair *node, int level);
void Cson_printASTtype(JsonParserValue *node, int level);

void Cson_printKeyVal(JsonKeyValuePair *node, int level);
void Cson_printAST(JsonParserValue *node, int level);

void Cson_free(JsonParserValue *node);


JsonParserValue *Cson_get(JsonParserValue *root, char *path);


JsonParserValue Cson_parseObject(RSStr *data, size_t *start);
JsonParserValue Cson_parseArray(RSStr *data, size_t *start);
JsonParserValue Cson_parseString(RSStr *data, size_t *start);
JsonParserValue Cson_parseBoolean(RSStr *data, size_t *start);
JsonParserValue Cson_parseNull(RSStr *data, size_t *start);
JsonParserValue Cson_parseNumeric(RSStr *data, size_t *start);
JsonParserValue Cson_loadData(RSStr *data);

#endif