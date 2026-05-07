#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "rsJSON.h"

#ifndef RSAPI
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
#endif


//-------------------------------------------------------------------

struct stack{
    RSJsonValue *values;
    int capacity;
    int size;
};

static struct stack createStack(int capacity){
    struct stack retval = {.size = 0, .capacity = capacity};
    retval.values = (RSJsonValue*)malloc(sizeof(RSJsonValue) * capacity);
    return retval;
}

static void stackPush(struct stack *stack, RSJsonValue value){
    if(stack->size >= stack->capacity){
        stack->capacity *= 2;
        stack->values = (RSJsonValue*)realloc(stack->values, sizeof(RSJsonValue) * stack->capacity);
    }
    stack->values[stack->size++] = value;
}

static RSJsonValue stackPop(struct stack *stack){
    if(stack->size){
        stack->size--;
    }
    if(stack->size){
        return stack->values[stack->size - 1];
    }
    return (RSJsonValue){.type = JSON_ERROR};
}

static void stackUpdate(struct stack *stack, RSJsonValue *value){
    if(stack->size){
        stack->values[stack->size - 1] = *value;
    }
}

//-------------------------------------------------------------------


//-------------------------------------------------------------------

static RSJsonValue createJSONObject(){
    int cap = 4;
    RSJsonValue retval = {
        .type = JSON_OBJECT,
        .data.object.size = 0,
        .data.object.capacity = cap,
        .data.object.keys = (RSStringView*)malloc(sizeof(RSStringView) * cap),
        .data.object.values = (RSJsonValue*)malloc(sizeof(RSJsonValue) * cap)
    };
    return retval;
}

static RSJsonValue createJSONOArray(){
    int cap = 4;
    RSJsonValue retval = {
        .type = JSON_ARRAY,
        .data.array.size = 0,
        .data.array.capacity = cap,
        .data.array.elements = (RSJsonValue*)malloc(sizeof(RSJsonValue) * cap)
    };
    return retval;
}

static void pushObjectKey(RSJsonValue *obj, RSJsonValue *val){
    if(obj->data.object.size >= obj->data.object.capacity){
        obj->data.object.capacity *= 2;
        obj->data.object.keys = (RSStringView*)realloc(obj->data.object.keys, sizeof(RSStringView) * obj->data.object.capacity);
        obj->data.object.values = (RSJsonValue*)realloc(obj->data.object.values, sizeof(RSJsonValue) * obj->data.object.capacity);
    }
    obj->data.object.keys[obj->data.object.size] = val->data.string;
    return;
}

static void pushObjectValue(RSJsonValue *obj, RSJsonValue *val){
    obj->data.object.values[obj->data.object.size++] = *val;
    return;
}

static void pushArrayValue(RSJsonValue *arr, RSJsonValue *val){
    if(arr->data.array.size >= arr->data.array.capacity){
        arr->data.array.capacity *= 2;
        arr->data.array.elements = (RSJsonValue*)realloc(arr->data.array.elements, sizeof(RSJsonValue) * arr->data.array.capacity);
    }
    arr->data.array.elements[arr->data.array.size++] = *val;
    return;
}

//-------------------------------------------------------------------


static bool isWhitespace(char *index){
    switch(*index){
        case '\t':
        case '\n':
        case '\r':
        case ' ':
            return true;
    }
    return false;
}

RSAPI void RSon_freeAST(RSJsonValue *root){
    switch(root->type){
        case JSON_ERROR:
        case JSON_BOOLEAN:
        case JSON_NULL:
        case JSON_NUMBER:
        case JSON_STRING:
            break;
        case JSON_ARRAY:
            for(int i = 0; i < root->data.array.size; i++){
                RSon_freeAST(root->data.array.elements + i);
            }
            free(root->data.array.elements);
            break;
        case JSON_OBJECT:
            for(int i = 0; i < root->data.object.size; i++){
                RSon_freeAST(root->data.object.values + i);
            }
            free(root->data.object.keys);
            free(root->data.object.values);
            break;
    }
    return;
}



RSAPI RSJsonValue RSon_parseNull(RSJsonContext *context){
    RSJsonValue retval = {.type = JSON_ERROR};
    char *start = context->current;
    if((*start == 'n') && (*(start+1) == 'u') && (*(start+2) == 'l') && (*(start+3) == 'l')){
        retval.type = JSON_NULL;
        context->current += 4;
    }

    return retval;
}

RSAPI RSJsonValue RSon_parseBoolean(RSJsonContext *context){
    RSJsonValue retval = {.type = JSON_ERROR};
    char *start = context->current;

    if((*start == 't') && (*(start+1) == 'r') && (*(start+2) == 'u') && (*(start+3) == 'e')){
        retval.type = JSON_BOOLEAN;
        retval.data.boolean = true;
        context->current += 4;
    }
    else if((*start == 'f') && (*(start+1) == 'a') && (*(start+2) == 'l') && (*(start+3) == 's') && (*(start+4) == 'e')){
        retval.type = JSON_BOOLEAN;
        retval.data.boolean = false;
        context->current += 5;
    }

    return retval;
}

RSAPI RSJsonValue parseString(RSJsonContext *context){
    char *start = context->current;
    char *end = context->end;
    RSStringView string = {.start = start, .end = NULL};
    RSJsonValue retval = {.type = JSON_ERROR};


    while(start < end){
        switch (*start) {
        case '\\':
            start++;
            break;
        case '\"':
            context->current = start + 1;
            string.end = start;
            retval.data.string = string;
            retval.type = JSON_STRING;
            return retval;
        default:
            break;
        }

        start++;
    }

    return retval;
}

RSAPI RSJsonValue RSon_parseNumber(RSJsonContext *context){
    char *start = context->current;
    RSJsonNumber number = {.start = start, .end = NULL, .is_integer = true, .is_numeric = true};
    RSJsonValue retval = {.type = JSON_ERROR};
    bool is_negative = false;
    
    if(*start == '-'){
        start++;
        is_negative = true;
    }
    
    if(*start == '0' && (*(start + 1) >= '0' && *(start + 1) <= '9')){
        return retval;
    }
    else if(*start < '0' || *start > '9'){
        return retval;
    }
    
    bool is_integer = true;
    bool is_numeric = true;
    long long int integer = 0;
    
    while((*start >= '0') && (*start <= '9')){
        char c = *start;
        integer *= 10;
        integer += c - '0';
        start++;
    }

    number.numeric.integer = is_negative ? -integer : integer;

    if(*start == '.'){
        number.is_integer = false;
        is_integer = false;
        start++;
        if(*start < '0' || *start > '9'){
            return retval;
        }
        double decimal = (double)integer;
        integer = 0;
        int divisor = 1;
        while((*start >= '0') && (*start <= '9')){
            char c = *start;
            divisor *= 10;
            integer *= 10;
            integer += c - '0';
            start++;            
        }
        decimal += (double)integer / (double)divisor;
        number.numeric.decimal = is_negative ? -decimal : decimal;
    }

    if(*start == 'e' || *start == 'E'){
        number.is_numeric = false;
        is_numeric = false;
        start++;
        if(*start == '+' || *start == '-'){
            start++;
        }

        if(*start < '0' || *start > '9'){
            return retval;
        }

        while((*start >= '0') && (*start <= '9')){
            start++;
        }
    }

    context->current = start;

    number.end = start;
    retval.data.number = number;
    retval.type = JSON_NUMBER;
    return retval;
}

RSAPI RSJsonValue RSon_loadDataFromString(char input[]){
    RSJsonValue root = {.type = JSON_ERROR};

    if(input == NULL) return root;

    
    RSJsonValue current = {.type = JSON_ERROR};
    char *end = input + strlen(input);
    char *index = input;
    char c;
    RSJsonContext context = {.start = input, .end = end, .current = input};
    RSJsonValue last_parent = {.type = JSON_ERROR};
    bool is_key = false;
    bool expecting_node = true;
    
    struct stack stack = createStack(10);
    

    while(index < end){
        while(isWhitespace(index)){
            index++;
        }
        c = *index;

        switch (c){
        case '{':       //open object
            if(is_key){ //this shouldn't happen, object can not be a key

            }
            
            index++;
            stackUpdate(&stack, &last_parent);
            last_parent = createJSONObject();
            stackPush(&stack, last_parent);
            is_key = true;
            continue;
            break;
        
        case '}':       //close object
            if(last_parent.type != JSON_OBJECT){
                goto free_ast;
            }
            index++;
            current = last_parent;
            last_parent = stackPop(&stack);
            is_key = false;
            goto save_node;
            break;
        case '[':       //open array
            if(is_key){ //this shouldn't happen, array can not be a key

            }

            index++;
            stackUpdate(&stack, &last_parent);
            last_parent = createJSONOArray();
            stackPush(&stack, last_parent);
            continue;
            break;
        case ']':       //close array
            if(last_parent.type != JSON_ARRAY){
                goto free_ast;
            }
            index++;
            current = last_parent;
            last_parent = stackPop(&stack);
            goto save_node;
            break;
        
        case '\"':      //string
            index++;
            context.current = index;
            current = parseString(&context);
            index = context.current;
            goto save_node;
            break;


        case 'f':       //booleans
        case 't':
            if(is_key){ //this shouldn't happen, boolean can not be a key

            }

            context.current = index;
            current = RSon_parseBoolean(&context);
            index = context.current;
            goto save_node;
            break;
        case 'n':       //null
            if(is_key){ //this shouldn't happen, null can not be a key

            }

            context.current = index;
            current = RSon_parseNull(&context);
            index = context.current;
            goto save_node;
            break;
        

        case '-':case '0':case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9':       //number
            if(is_key){ //this shouldn't happen, number can not be a key

            }

            context.current = index;
            current = RSon_parseNumber(&context);
            index = context.current;
            goto save_node;
            break;

        default:
            if(expecting_node == true || last_parent.type == JSON_ERROR){
                goto free_ast;
            }
            else if(c == ',' && (is_key || last_parent.type != JSON_OBJECT)){
                index++;
                expecting_node = true;
            }
            else if(c == ':' && !is_key){
                index++;
                expecting_node = true;
            }
            else{
                goto free_ast;
            }
            break;
        }

        
        continue;
        
        save_node:
        if(current.type == JSON_ERROR){
            goto free_ast;
        }

        switch(last_parent.type){
            case JSON_ARRAY:
                pushArrayValue(&last_parent, &current);
                break;
            case JSON_OBJECT:
                if(is_key){
                    pushObjectKey(&last_parent, &current);
                }
                else{
                    pushObjectValue(&last_parent, &current);
                }
                is_key = !is_key;
                break;
            default:
                break;
        }
        expecting_node = false;
    }

    free(stack.values);

    root = current;

    return root;

    free_ast:
    if(last_parent.type != JSON_ERROR) root = stack.values[0];
    else root = current;
    RSon_freeAST(&root);
    free(stack.values);
    return (RSJsonValue){.type = JSON_ERROR};

}


RSAPI RSJsonValue *RSon_getPath(RSJsonValue *root, char *path){
    if(!root) return NULL;

    RSJsonValue *node = root;

    int length = 0;
    int index = 0;
    while(path[length] != '\0') length++;
    
    char character;
    int json_arr_index = 0;

    while(index < length){
        character = path[index];
        switch (character){
        case '[':
            if(node->type != JSON_ARRAY){
                printf("data type is not a Json array\n");
                return NULL;
            }
            index++;
            if(path[index] > '9' || path[index] < '0'){
                printf("incorrect Json array index\n");
                return NULL;
            }
            json_arr_index = 0;
            break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            do{
                json_arr_index = json_arr_index * 10 + character - '0';
                index++;
                character = path[index];
            }while(character <= '9' && character >= '0');
            if(character != ']') {
                printf("incorrect Json array index\n");
                return NULL;
            }
            break;
        case ']':
            if(json_arr_index >= node->data.array.size){
                printf("incorrect array index - out of bounds\n");
                return NULL;
            }
            node = node->data.array.elements + json_arr_index;
            index++;
            if(path[index] != '.' && path[index] != '\0'){
                printf("incorrect path\n");
                return NULL;
            }
            break;
        case '.':
            index++;
            if(path[index] == '['){
                break;
            }

        default:
            if(node->type != JSON_OBJECT){
                printf("incorrect path, node is not an object\n");
                return NULL;
            }

            int start = index;
            while(index < length){
                character = path[index];
                if(character == '.'){
                    break;
                }
                index++;
            }
            
            for(size_t i = 0; i < node->data.object.size; i++){
                RSJsonValue *kp;
                kp = node->data.object.values + i;
                char *t = (node->data.object.keys + i)->start;
                char *e = (node->data.object.keys + i)->end;
                for(int i = start; i < index; i++){
                    if(*t == '\0' || path[i] != *t){
                        goto get_next_key;   
                    }
                    t++;
                }
                
                if(*t == '\0' || *t == '\"'){
                    node = kp;
                    goto key_found;
                }
                
                get_next_key:
            }

            printf("incorrect path : \"%s\" key not found\n", path + start);
            return NULL;
            key_found:
            break;
        }
    }
    return node;
}

RSAPI RSJsonValue *RSon_get(RSJsonValue *root, ...){
    if(!root) return NULL;

    va_list args;
    va_start(args, root);

    RSonPath path;
    while(1){
        path = va_arg(args, RSonPath);
        switch(path.type){
            case RSON_PATH_INDEX:
                if(root->type != JSON_ARRAY) {
                    return NULL;
                }

                if(root->data.array.size <= path.index) {
                    return NULL;
                }
                root = root->data.array.elements + path.index;
                break;
            case RSON_PATH_KEY:
                if(root->type != JSON_OBJECT || path.key == NULL) {
                    return NULL;
                }

                int length = strlen(path.key);
                bool found_match = false;
                for(int i = 0; i < root->data.object.size; i++){
                    int key_length = (root->data.object.keys + i)->end - (root->data.object.keys + i)->start;
                    if(length != key_length) continue;
                    char *key = (root->data.object.keys + i)->start;
                    int j;
                    for(j = 0; j < length; j++){
                        if(*key != path.key[j]){
                            break;
                        }
                        key++;
                    }
                    if(j == length){
                        root = root->data.object.values + i;
                        found_match = true;
                        break;
                    }
                }
                if(!found_match) return NULL;
                break;
            case RSON_PATH_END:
                return root;
            default:
                break;
        }
    }

    return root;
}


RSAPI void RSon_printASTKeyValtype(RSJsonValue *node, int level){
    for(int i = 0; i < level; i++){
        printf("  ");
    }
    printf("type = JSON_STRING : ");
    switch(node->type){
        case JSON_ERROR:
            printf("type = JSON_ERROR");
            break;
        case JSON_BOOLEAN:
            printf("type = JSON_BOOLEAN");
            break;
        case JSON_NULL:
            printf("type = JSON_NULL");
            break;
        case JSON_NUMBER:
            printf("type = JSON_NUMBER");
            break;
        case JSON_STRING:
            printf("type = JSON_STRING");
            break;
        case JSON_ARRAY:
            printf("type = JSON_ARRAY[\n");
            for(int i = 0; i < node->data.array.size; i++){
                RSon_printASTtype(node->data.array.elements + i, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("]");
            break;
        case JSON_OBJECT:
            printf("type = JSON_OBJECT{\n");
            for(int i = 0; i < node->data.object.size; i++){
                RSon_printASTKeyValtype(node->data.object.values + i, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("}");
            break;
    }
    printf("\n");
    return;
}

RSAPI void RSon_printASTtype(RSJsonValue *node, int level){
    for(int i = 0; i < level; i++){
        printf("  ");
    }
    switch(node->type){
        case JSON_ERROR:
            printf("type = JSON_ERROR");
            break;
        case JSON_BOOLEAN:
            printf("type = JSON_BOOLEAN");
            break;
        case JSON_NULL:
            printf("type = JSON_NULL");
            break;
        case JSON_NUMBER:
            printf("type = JSON_NUMBER");
            break;
        case JSON_STRING:
            printf("type = JSON_STRING");
            break;
        case JSON_ARRAY:
            printf("type = JSON_ARRAY[\n");
            for(int i = 0; i < node->data.array.size; i++){
                RSon_printASTtype(node->data.array.elements + i, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("]");
            break;
        case JSON_OBJECT:
            printf("type = JSON_OBJECT{\n");
            for(int i = 0; i < node->data.object.size; i++){
                RSon_printASTKeyValtype(node->data.object.values + i, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("}");
            break;
    }
    printf("\n");
    return;
}


static void printLengthBasedString(char *start, char *end){
    if(end == NULL){
        while(*start != '\"' && *start){
            putc(*start, stdout);
            start++;
        }
    }
    else{
        while(start < end){
            putc(*start, stdout);
            start++;
        }
    }
}

RSAPI void RSon_printASTKeyValvalue(RSJsonValue *node, char *key_start, int level){
    for(int i = 0; i < level; i++){
        printf("  ");
    }
    printLengthBasedString(key_start, NULL);
    printf(" : ");
    switch(node->type){
        case JSON_ERROR:
            printf("JSON_ERROR");
            break;
        case JSON_BOOLEAN:
            printf("type = JSON_BOOLEAN");
            break;
        case JSON_NULL:
            printf("type = null");
            break;
        case JSON_NUMBER:
            printLengthBasedString(node->data.number.start, node->data.number.end);
            break;
        case JSON_STRING:
            printLengthBasedString(node->data.string.start, node->data.string.end);
            break;
        case JSON_ARRAY:
            printf("type = JSON_ARRAY[\n");
            for(int i = 0; i < node->data.array.size; i++){
                RSon_printASTvalue(node->data.array.elements + i, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("]");
            break;
        case JSON_OBJECT:
            printf("{\n");
            for(int i = 0; i < node->data.object.size; i++){
                RSon_printASTKeyValvalue(node->data.object.values + i, node->data.object.keys[i].start, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("}");
            break;
    }
    printf("\n");
    return;
}

RSAPI void RSon_printASTvalue(RSJsonValue *node, int level){
    for(int i = 0; i < level; i++){
        printf("  ");
    }
    switch(node->type){
        case JSON_ERROR:
            printf("JSON_ERROR");
            break;
        case JSON_BOOLEAN:
            if(node->data.boolean) printf("true");
            else printf("false");
            break;
        case JSON_NULL:
            printf("null");
            break;
        case JSON_NUMBER:
            printLengthBasedString(node->data.number.start, node->data.number.end);
            break;
        case JSON_STRING:
            printLengthBasedString(node->data.string.start, node->data.string.end);
            break;
        case JSON_ARRAY:
            printf("[\n");
            for(int i = 0; i < node->data.array.size; i++){
                RSon_printASTvalue(node->data.array.elements + i, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("]");
            break;
        case JSON_OBJECT:
            printf("{\n");
            for(int i = 0; i < node->data.object.size; i++){
                RSon_printASTKeyValvalue(node->data.object.values + i, node->data.object.keys[i].start, level + 1);
            }
            for(int i = 0; i < level; i++){
                printf("  ");
            }
            printf("}");
            break;
    }
    printf("\n");
    return;
}



RSAPI void RSon_printNode(RSJsonValue *node){
    if(!node) return;
    switch(node->type){
        case JSON_ERROR:
            printf("JSON_ERROR\n");
            break;
        case JSON_BOOLEAN:
            if(node->data.boolean) printf("true\n");
            else printf("false\n");
            break;
        case JSON_NULL:
            printf("null\n");
            break;
        case JSON_NUMBER:
            char *e = node->data.number.end;
            for(char *s = node->data.number.start; s < node->data.number.end; s++){
                putc(*s, stdout);
            }
            putc('\n', stdout);
            break;
        case JSON_STRING:
            for(char *s = node->data.string.start; s < node->data.string.end; s++){
                putc(*s, stdout);
            }
            putc('\n', stdout);
            break;
        case JSON_ARRAY:
            printf("JSON_ARRAY\n");
            break;
        case JSON_OBJECT:
            printf("JSON_OBJECT\n");
            break;
        default:
            break;
    }
    return;
}

