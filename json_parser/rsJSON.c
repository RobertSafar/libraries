#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
    // memcpy(stack->values + (sizeof(RSJsonValue) * stack->size), &value, sizeof(RSJsonValue));
    // stack->size++;
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
        .data.object.keys = (char**)malloc(sizeof(char*) * cap),
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
        obj->data.object.keys = (char**)realloc(obj->data.object.keys, sizeof(char*) * obj->data.object.capacity);
        obj->data.object.values = (RSJsonValue*)realloc(obj->data.object.values, sizeof(RSJsonValue) * obj->data.object.capacity);
    }
    obj->data.object.keys[obj->data.object.size] = val->data.string.start;
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

RSAPI void freeAST(RSJsonValue *root){
    switch(root->type){
        case JSON_ERROR:
        case JSON_BOOLEAN:
        case JSON_NULL:
        case JSON_NUMBER:
        case JSON_STRING:
            break;
        case JSON_ARRAY:
            for(int i = 0; i < root->data.array.size; i++){
                freeAST(root->data.array.elements + i);
            }
            free(root->data.array.elements);
            break;
        case JSON_OBJECT:
            for(int i = 0; i < root->data.object.size; i++){
                freeAST(root->data.object.values + i);
            }
            free(root->data.object.keys);
            free(root->data.object.values);
            break;
    }
    return;
}



RSAPI RSJsonValue parseNull(char *start, char **index){
    RSJsonValue retval = {.type = JSON_ERROR};
    
    if((*start == 'n') && (*(start+1) == 'u') && (*(start+2) == 'l') && (*(start+3) == 'l')){
        retval.type = JSON_NULL;
        *index += 4;
    }

    return retval;
}

RSAPI RSJsonValue parseBoolean(char *start, char **index){
    RSJsonValue retval = {.type = JSON_ERROR};

    if((*start == 't') && (*(start+1) == 'r') && (*(start+2) == 'u') && (*(start+3) == 'e')){
        retval.type = JSON_BOOLEAN;
        retval.data.boolean = true;
        *index += 4;
    }
    else if((*start == 'f') && (*(start+1) == 'a') && (*(start+2) == 'l') && (*(start+3) == 's') && (*(start+4) == 'e')){
        retval.type = JSON_BOOLEAN;
        retval.data.boolean = false;
        *index += 5;
    }

    return retval;
}

RSAPI RSJsonValue parseString(char *start, char *end, char **index){
    RSStringView string = {.start = start, .end = NULL, .owned = false};
    RSJsonValue retval = {.type = JSON_ERROR};


    while(start < end){
        switch (*start) {
        case '\\':
            start++;
            break;
        case '\"':
            *index = start + 1;
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

RSAPI RSJsonValue parseNumber(char *start, char *end, char **index){ //index doesn't get updated and so there is an infinite loop
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


    // while(start < end){
    //     char c = *start;
    //     if((c >= '0') && (c <= '9')){
    //     }
    //     else if(c == '.' && integer){
    //         if(*(start + 1) >= '0' && *(start + 1) <= '9'){
    //             return retval;
    //         }
    //         integer = false;
    //     }
    //     else {
    //         number.end = start;
    //         retval.type = JSON_NUMBER;
    //         retval.data.number = number;
    //         return retval;
    //     }
    //     start++;
    // }

    *index = start;

    number.end = start;
    retval.data.number = number;
    retval.type = JSON_NUMBER;
    return retval;
}

RSAPI RSJsonValue loadDataFromString(char input[]){
    RSJsonValue root = {.type = JSON_ERROR};

    if(input == NULL) return root;

    RSJsonValue current = {.type = JSON_ERROR};
    char *end = input + strlen(input);
    char *index = input;
    char c;
    RSJsonValue last_parent = {.type = JSON_ERROR};
    bool is_key = false;
    bool expecting_node = true;
    
    // RSJsonValue *stack = (RSJsonValue*)malloc(sizeof(RSJsonValue) * 10);
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
            current = parseString(index, end, &index);
            goto save_node;
            break;


        case 'f':       //booleans
        case 't':
            if(is_key){ //this shouldn't happen, boolean can not be a key

            }

            current = parseBoolean(index, &index);
            goto save_node;
            break;
        case 'n':       //null
            if(is_key){ //this shouldn't happen, null can not be a key

            }

            current = parseNull(index, &index);
            goto save_node;
            break;
        

        case '-':case '0':case '1':case '2':case '3':case '4':
        case '5':case '6':case '7':case '8':case '9':       //number
            if(is_key){ //this shouldn't happen, number can not be a key

            }

            current = parseNumber(index, end, &index);
            goto save_node;
            break;

        default:
            if(expecting_node == true || last_parent.type == JSON_ERROR){
                // freeAST(&root);
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
                // freeAST(&root);
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

    // if(last_parent.type != JSON_ERROR) root = last_parent;
    // else root = current;
    root = current;

    return root;

    free_ast:
    if(last_parent.type != JSON_ERROR) root = stack.values[0];
    else root = current;
    freeAST(&root);
    free(stack.values);
    return (RSJsonValue){.type = JSON_ERROR};

}


void RSon_printASTKeyValtype(RSJsonValue *node, int level){
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
                // printf("type = JSON_STRING : ");
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

void RSon_printASTtype(RSJsonValue *node, int level){
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
                // printf("type = JSON_STRING : ");
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