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



typedef enum{
    RSON_PATH_INDEX,
    RSON_PATH_KEY,
    RSON_PATH_END
}RSJsonPathType;


typedef struct RSonPath{
    RSJsonPathType type;
    union{
        char* key;
        size_t index;
    };
}RSonPath;

#define RSON_INDEX(i) ((RSonPath){.type = RSON_PATH_INDEX, .index = (i)})
#define RSON_KEY(k) ((RSonPath){.type = RSON_PATH_KEY, .key = (k)})
#define RSON_END ((RSonPath){.type = RSON_PATH_END})

void RSon_freeAST(RSJsonValue *root);

RSJsonValue RSon_parseNull(RSJsonContext *context);

RSJsonValue RSon_parseBoolean(RSJsonContext *context);

RSJsonValue RSon_parseString(RSJsonContext *context);

RSJsonValue RSon_parseNumber(RSJsonContext *context);

RSJsonValue RSon_loadDataFromString(char input[]);

RSJsonValue *RSon_getPath(RSJsonValue *root, char *path);
RSJsonValue *RSon_get(RSJsonValue *root, ...);

void RSon_printASTKeyValtype(RSJsonValue *node, int level);
void RSon_printASTtype(RSJsonValue *node, int level);

void RSon_printASTKeyValvalue(RSJsonValue *node, char *key_start, int level);
void RSon_printASTvalue(RSJsonValue *node, int level);

void RSon_printNode(RSJsonValue *node);

#endif