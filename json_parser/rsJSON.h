#ifndef RS_JSON
#define RS_JSON

#include <stdbool.h>

typedef struct{
    char *start;
    char *end;
    char *current;
}RSJsonContext;


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
            RSStringView *keys;
            struct RSJsonValue *values;
        }object;
        
    }data;
}RSJsonValue;


void RSon_freeAST(RSJsonValue *root);

RSJsonValue RSon_parseNull(RSJsonContext *context);

RSJsonValue RSon_parseBoolean(RSJsonContext *context);

RSJsonValue RSon_parseString(RSJsonContext *context);

RSJsonValue RSon_parseNumber(RSJsonContext *context);

RSJsonValue RSon_loadDataFromString(char input[]);


void RSon_printASTKeyValtype(RSJsonValue *node, int level);
void RSon_printASTtype(RSJsonValue *node, int level);

void RSon_printASTKeyValvalue(RSJsonValue *node, char *key_start, int level);
void RSon_printASTvalue(RSJsonValue *node, int level);

#endif