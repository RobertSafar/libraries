#ifndef RS_JSON
#define RS_JSON

#include <stdbool.h>

typedef enum{
    JSON_ERROR,
    JSON_BOOLEAN,
    JSON_NULL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
}RSJsonTypes;


#ifndef RS_STRING_VIEW
#define RS_STRING_VIEW
typedef struct RSStringView {
    char *start;
    char *end;
    bool owned;
}RSStringView;
#endif

typedef struct RSJsonNumber{
    bool is_numeric;
    bool is_integer;
    bool is_copied;
    union {
        long long int integer;
        double decimal;
    }numeric;
    char *start;
    char *end;
}RSJsonNumber;

typedef struct RSJsonValue {
    RSJsonTypes type;
    union {
        bool boolean;
        struct RSJsonNumber number;
        struct RSStringView string;
        struct {
            int size;
            int capacity;
            struct RSJsonValue *elements;
        }array;
        struct {
            int size;
            int capacity;
            char **keys;
            struct RSJsonValue *values;
        }object;
        
    }data;
}RSJsonValue;


void freeAST(RSJsonValue *root);

RSJsonValue parseNull(char *start, char **index);

RSJsonValue parseBoolean(char *start, char **index);

RSJsonValue parseString(char *start, char *end, char **index);

RSJsonValue parseNumber(char *start, char *end, char **index);

RSJsonValue loadDataFromString(char input[]);

void RSon_printASTKeyValtype(RSJsonValue *node, int level);

void RSon_printASTtype(RSJsonValue *node, int level);

#endif