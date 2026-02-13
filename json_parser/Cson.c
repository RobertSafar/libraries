#include "Cson.h"


#ifdef _WIN32
    #ifdef BUILDING_DLL
        #define CSAPI __declspec(dllexport)
    #elif defined(USING_DLL)
        #define CSONAPI __declspec(dllimport)
    #else
        #define CSAPI
    #endif
#else
  #define CSAPI   /* nothing */
#endif


CSAPI void Cson_printKeyVal(JsonKeyValuePair *node, int level){
    for(int i = 0; i < level; i++){
        putchar('\t');
    }
    printf("type = JSON_STRING : ");
    switch (node->value.type){
    case JSON_ERROR:
        printf("type = JSON_ERROR");
        break;
    case JSON_STRING:
        printf("type = JSON_STRING");
        break;
    case JSON_INT:
        printf("type = JSON_INT");
        break;
    case JSON_DOUBLE:
        printf("type = JSON_DOUBLE");
        break;
    case JSON_BOOLEAN:
        printf("type = JSON_BOOLEAN");
        break;
    case JSON_NULL:
        printf("type = JSON_NULL");
        break;
    case JSON_OBJECT:
        printf("type = JSON_OBJECT");
        break;
    case JSON_ARRAY:
        printf("type = JSON_ARRAY");
        break;
    default:
        break;
    }
    return;
}

CSAPI void Cson_printAST(JsonParserValue *node, int level){
    for(int i = 0; i < level; i++){
        putchar('\t');
    }
    switch (node->type){
    case JSON_ERROR:
        printf("type = JSON_ERROR");
        break;
    case JSON_STRING:
        printf("type = JSON_STRING");
        break;
    case JSON_INT:
        printf("type = JSON_INT");
        break;
    case JSON_DOUBLE:
        printf("type = JSON_DOUBLE");
        break;
    case JSON_BOOLEAN:
        printf("type = JSON_BOOLEAN");
        break;
    case JSON_NULL:
        printf("type = JSON_NULL");
        break;
    case JSON_OBJECT:
        printf("type = JSON_OBJECT{\n");
        for(int i = 0; i < node->val.object.size - 1; i++){
            Cson_printKeyVal(CVector_getPointer(&node->val.object, i), level + 1);
            printf(",\n");
        }
        Cson_printKeyVal(CVector_getPointer(&node->val.object, node->val.object.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            putchar('\t');
        }
        printf("}\n");
        break;
    case JSON_ARRAY:
        printf("type = JSON_ARRAY[\n");
        for(int i = 0; i < node->val.array.size - 1; i++){
            Cson_printAST(CVector_getPointer(&node->val.array, i), level + 1);
            printf(",\n");
        }
        Cson_printAST(CVector_getPointer(&node->val.array, node->val.array.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            putchar('\t');
        }
        printf("]\n");
        break;
    default:
        break;
    }
    return;
}

CSAPI void Cson_free(JsonParserValue *node){
    switch(node->type){
        case JSON_BOOLEAN:
        case JSON_DOUBLE:
        case JSON_INT:
        case JSON_NULL:
        case JSON_ERROR:
            return;

        case JSON_STRING:
            String_destroy(&node->val.string);
            break;
        case JSON_ARRAY:
            for(int i = 0; i < CVector_getSize(&node->val.array); i++){
                Cson_free(CVector_getPointer(&node->val.array, i));
                CVector_destroy(&node->val.array);
            }
        case JSON_OBJECT:
            for(int i = 0; i < CVector_getSize(&node->val.array); i++){
                JsonKeyValuePair *p = CVector_getPointer(&node->val.object, i);
                String_destroy(&p->key);
                Cson_free(&p->value);
                CVector_destroy(&node->val.object);
            }
        break;
    }
    return;
}

CSAPI JsonParserValue Cson_getObject(String *data, size_t *start){
    size_t input_size = data->size;
    size_t index = *start;
    JsonParserValue ret_val;
    ret_val.type = JSON_OBJECT;
    ret_val.val.object = CVECTOR_EMPTY(JsonKeyValuePair, NULL, NULL);
    JsonKeyValuePair keyVal_pair;
    char character;
    if(character == '}'){
        return ret_val;
    }
    while(index < input_size){
        String_get(data, index, &character);
        //I have to check if the object has been closed
        if(character !='\"'){
            //free here
            Cson_free(&ret_val);
            ret_val.type = JSON_ERROR;
            return ret_val;
        }
        index++;
        JsonParserValue temp = Cson_getString(data, &index);
        if(temp.type == JSON_ERROR){
            //free here
            Cson_free(&ret_val);
            String_destroy(&temp.val.string);
            ret_val.type = JSON_ERROR;
            return ret_val;
        }
        keyVal_pair.key = temp.val.string;
        String_get(data, index, &character);

        if(character != ':'){
            //free here
            Cson_free(&ret_val);
            String_destroy(&temp.val.string);
            ret_val.type = JSON_ERROR;
            return ret_val;
        }
        index++;
        String_get(data, index, &character);



        switch (character) {
        case '\"':
            index++;
            temp = Cson_getString(data, &index);
            break;
        case '[':
            index++;
            temp = Cson_getArray(data, &index);
            break;
        case '{':
             index++;
            temp = Cson_getObject(data, &index);
            break;    
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            temp = Cson_getNumeric(data, &index);
            break;
        case 't': case 'f':
            temp = Cson_getBoolean(data, &index);
            break;
        case 'n':
            temp = Cson_getNull(data, &index);
            break;
        default:
            Cson_free(&ret_val);
            String_destroy(&temp.val.string);
            ret_val.type = JSON_ERROR;
            return ret_val;
            break;
        }

        keyVal_pair.value = temp;
        CVector_push(&ret_val.val.object, &keyVal_pair);
        String_get(data, index, &character);
        if(character == '}' ){
            goto return_object;
            // return ret_val;
        }
        else if(character != ','){
            Cson_free(&ret_val);
            return ret_val;
        }

        index++;
    }
    return_object:
    *start = ++index;
    return ret_val;
}

CSAPI JsonParserValue Cson_getArray(String *data, size_t *start){
    size_t index = *start;
    size_t input_size = data->size;
    JsonParserValue ret_val;
    JsonParserValue temp;
    ret_val.type = JSON_ARRAY;
    ret_val.val.object = CVECTOR_EMPTY(JsonParserValue, NULL, NULL);
    char character;
    String_get(data, index, &character);
    // if(character == ']'){
    //     *start = ++index;
    //     return ret_val;
    // }
    if(character == ','){
        ret_val.type = JSON_ERROR;
        return ret_val;
    }

    while(index < input_size){
        String_get(data, index, &character);
        switch (character){
        case ',':
            index++;
            continue;
            break;
        case '\"':
            index++;
            temp = Cson_getString(data, &index);
            break;
        case '[':
            temp = Cson_getArray(data, &index);
            break;
        case '{':
            index++;
            temp = Cson_getObject(data, &index);
            break;
        case 't': case 'f':
            temp = Cson_getBoolean(data, &index);
            break;
        case 'n':
            temp = Cson_getNull(data, &index);
            break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            temp = Cson_getNumeric(data, &index);
            break;
        case ']':
            *start = ++index;
            return ret_val;
            break;
        default:
            ret_val.type = JSON_ERROR;
            return ret_val;
            break;
        }

        if(temp.type == JSON_ERROR){
            Cson_free(&temp);
            Cson_free(&ret_val);
            ret_val.type = JSON_ERROR;
            return ret_val;
        }
        CVector_push(&ret_val.val.array, &temp);
        // index++;
    }
    return ret_val;
}

CSAPI JsonParserValue Cson_getString(String *data, size_t *start){
    size_t input_size = data->size;
    size_t index = *start;

    char temp;    
    while(index < input_size){
        char character;
        String_get(data, index, &character);
        switch (character) {
            case '\\':
                if(index + 1 == input_size){
                    return (JsonParserValue){.type = JSON_ERROR};
                }
                break;
            case '\0':
                return (JsonParserValue){.type = JSON_ERROR};
                break;
            case '\"':
                temp = data->text[index];
                data->text[index] = '\0';
                goto return_string;
                break;
            default:
                break;
        }
        index++;
    }

    return_string:
    JsonParserValue ret_val = {
        .type = JSON_STRING,
        .val.string = String_createAssign(data->text + *start)
    };
    data->text[index] = temp;
    *start = index + 1;
    return ret_val;
}

CSAPI JsonParserValue Cson_getBoolean(String *data, size_t *start){
    size_t index = *start;
    char *text = data->text + index;
    JsonParserValue ret_val = {
        .type = JSON_BOOLEAN
    };
    if((*text == 't') && (*(text+1) == 'r') && (*(text+2) == 'u') && (*(text+3) == 'e')){
        ret_val.val.boolean = true;
        *start += 4;
    }
    else if((*text == 'f') && (*(text+1) == 'a') && (*(text+2) == 'l') && (*(text+3) == 's') && (*(text+4) == 'e')){
        ret_val.val.boolean = false;
        *start += 5;
    }
    else{
        ret_val.type = JSON_ERROR;
    }
    return ret_val;
}

CSAPI JsonParserValue Cson_getNull(String *data, size_t *start){
    size_t index = *start;
    char *text = data->text + index;
    JsonParserValue ret_val = {
        .type = JSON_NULL
    };
    if((*text == 'n') && (*(text+1) == 'u') && (*(text+2) == 'l') && (*(text+3) == 'l')){
        *start += 4;
    }
    else{
        ret_val.type = JSON_ERROR;
    }
    return ret_val;
}

CSAPI JsonParserValue Cson_getNumeric(String *data, size_t *start){
    size_t input_size = data->size;
    size_t index = *start;

    char temp;
    int number = 0;
    double decimal = 0;
    char character;
    JsonParserValue ret_val;
    while(index < input_size){
        String_get(data, index, &character);
        
        if(character >= '0' && character <= '9'){
            number *= 10;
            number += character - '0';
        }
        else if(character == '.'){
            index++;
            break;
        }
        else if(character == ',' || character == '}' || character ==']'){
            goto return_number;
        }
        else{
            return (JsonParserValue){
                .type = JSON_ERROR
            };
        }
        index++;
    }
    if(index == input_size) goto return_number;
    parse_decimal:
    String_get(data, index, &character);
    if(character < '0' || character > '9'){
        return (JsonParserValue){
            .type = JSON_ERROR
        };
    }
    decimal = (double)number;
    int divisor = 1;
    number = 0;
    while(index < input_size){
        char character;
        String_get(data, index, &character);
        
        if(character >= '0' && character <= '9'){
            divisor *= 10;
            number *= 10;
            number += character - '0';
        }
        else if(character == ',' || character == '}' || character ==']'){
            goto return_decimal;
        }
        else{
            return (JsonParserValue){
                .type = JSON_ERROR
            };
        }

        index++;
    }
    
    return_decimal:
    *start = index;
    decimal += (double)number / (double)divisor;
    ret_val.type = JSON_DOUBLE;
    ret_val.val.decimal = decimal;
    return ret_val;
    
    return_number:
    *start = index;
    ret_val.type = JSON_INT;
    ret_val.val.number = number;
    return ret_val;
}

CSAPI JsonParserValue Cson_loadData(String *data){
    if(!data || !data->size ) return (JsonParserValue){.type = JSON_ERROR};
    String white_spaces = String_createAssign(" \n\t\r");
    String_removeCharacterSet(data, &white_spaces);
    String_destroy(&white_spaces);

    size_t input_size = data->size;
    size_t index = 0;
    JsonParserValue ret_val;
    // while(index < input_size){
        char character;
        String_get(data, index, &character);
        switch (character) {
        case '\"':
            index++;
            ret_val = Cson_getString(data, &index);
            break;
        case '[':
            index++;
            ret_val = Cson_getArray(data, &index);
            break;
        case '{':
            index++;
            ret_val = Cson_getObject(data, &index);
            break;    
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            ret_val = Cson_getNumeric(data, &index);
            break;
        case 't': case 'f':
            ret_val = Cson_getBoolean(data, &index);
            break;
        case 'n':
            ret_val = Cson_getNull(data, &index);
            break;
        default:
            break;
        // }

        // index++;
    }

    return ret_val;
}