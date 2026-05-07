#include "Cson.h"


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

RSAPI void Cson_printNode(JsonParserValue *node){
    if(!node) return;
    switch (node->type){
    case JSON_ERROR:
        printf("JSON_ERROR");
        break;
    case JSON_STRING:
        printf("%s", node->val.string.text);
        break;
    case JSON_INT:
        printf("%d", node->val.number);
        break;
    case JSON_DOUBLE:
        printf("%lf", node->val.decimal);
        break;
    case JSON_BOOLEAN:
        if(node->val.boolean){
            printf("true");
        }
        else{
            printf("false");
        }
        break;
    case JSON_NULL:
        printf("null");
        break;
    case JSON_OBJECT:
        printf("JSON_OBJECT\n");
        break;
    case JSON_ARRAY:
        printf("JSON_ARRAY\n");
        break;
    default:
        break;
    }
    return;
}

RSAPI void Cson_printKeyValtype(JsonKeyValuePair *node, int level){
    for(int i = 0; i < level; i++){
        //putchar('\t');
        printf("    ");
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
        printf("type = JSON_OBJECT{\n");
        for(int i = 0; i < node->value.val.object.size - 1; i++){
            Cson_printKeyValtype(RSmvec_getPointer(&node->value.val.object, i), level + 1);
            printf(",\n");
        }
        Cson_printKeyValtype(RSmvec_getPointer(&node->value.val.object, node->value.val.object.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("}");
        break;
    case JSON_ARRAY:
        printf("type = JSON_ARRAY[\n");
        for(int i = 0; i < node->value.val.array.size - 1; i++){
            Cson_printASTtype(RSmvec_getPointer(&node->value.val.array, i), level + 1);
            printf(",\n");
        }
        Cson_printASTtype(RSmvec_getPointer(&node->value.val.array, node->value.val.array.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("]");
        break;
    default:
        break;
    }
    return;
}

RSAPI void Cson_printASTtype(JsonParserValue *node, int level){
    for(int i = 0; i < level; i++){
        //putchar('\t');
        printf("    ");
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
            Cson_printKeyValtype(RSmvec_getPointer(&node->val.object, i), level + 1);
            printf(",\n");
        }
        Cson_printKeyValtype(RSmvec_getPointer(&node->val.object, node->val.object.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("}\n");
        break;
    case JSON_ARRAY:
        printf("type = JSON_ARRAY[\n");
        for(int i = 0; i < node->val.array.size - 1; i++){
            Cson_printASTtype(RSmvec_getPointer(&node->val.array, i), level + 1);
            printf(",\n");
        }
        Cson_printASTtype(RSmvec_getPointer(&node->val.array, node->val.array.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("]\n");
        break;
    default:
        break;
    }
    return;
}

RSAPI void Cson_printKeyVal(JsonKeyValuePair *node, int level){
    for(int i = 0; i < level; i++){
        //putchar('\t');
        printf("    ");
    }
    printf("\"%s\" : ", node->key.text);
    switch (node->value.type){
    case JSON_ERROR:
        printf("JSON_ERROR");
        break;
    case JSON_STRING:
        printf("\"%s\"", node->value.val.string.text);
        break;
    case JSON_INT:
        printf("%d", node->value.val.number);
        break;
    case JSON_DOUBLE:
        printf("%lf", node->value.val.decimal);
        break;
    case JSON_BOOLEAN:
        if(node->value.val.boolean){
            printf("true");
        }
        else{
            printf("false");
        }
        break;
    case JSON_NULL:
        printf("null");
        break;
    case JSON_OBJECT:
        printf("{\n");
        for(int i = 0; i < node->value.val.object.size - 1; i++){
            Cson_printKeyVal(RSmvec_getPointer(&node->value.val.object, i), level + 1);
            printf(",\n");
        }
        Cson_printKeyVal(RSmvec_getPointer(&node->value.val.object, node->value.val.object.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("}");
        break;
    case JSON_ARRAY:
        printf("[\n");
        for(int i = 0; i < node->value.val.array.size - 1; i++){
            Cson_printAST(RSmvec_getPointer(&node->value.val.array, i), level + 1);
            printf(",\n");
        }
        Cson_printAST(RSmvec_getPointer(&node->value.val.array, node->value.val.array.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("]");
        break;
    default:
        break;
    }
    return;
}

RSAPI void Cson_printAST(JsonParserValue *node, int level){
    for(int i = 0; i < level; i++){
        //putchar('\t');
        printf("    ");
    }
    switch (node->type){
    case JSON_ERROR:
        printf("JSON_ERROR");
        break;
    case JSON_STRING:
        printf("%s", node->val.string.text);
        break;
    case JSON_INT:
        printf("%d", node->val.number);
        break;
    case JSON_DOUBLE:
        printf("%lf", node->val.decimal);
        break;
    case JSON_BOOLEAN:
        if(node->val.boolean){
            printf("true");
        }
        else{
            printf("false");
        }
        break;
    case JSON_NULL:
        printf("null");
        break;
    case JSON_OBJECT:
        printf("{\n");
        for(int i = 0; i < node->val.object.size - 1; i++){
            Cson_printKeyVal(RSmvec_getPointer(&node->val.object, i), level + 1);
            printf(",\n");
        }
        Cson_printKeyVal(RSmvec_getPointer(&node->val.object, node->val.object.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("}\n");
        break;
    case JSON_ARRAY:
        printf("[\n");
        for(int i = 0; i < node->val.array.size - 1; i++){
            Cson_printAST(RSmvec_getPointer(&node->val.array, i), level + 1);
            printf(",\n");
        }
        Cson_printAST(RSmvec_getPointer(&node->val.array, node->val.array.size - 1), level + 1);
        printf("\n");
        for(int i = 0; i < level; i++){
            //putchar('\t');
            printf("    ");
        }
        printf("]\n");
        break;
    default:
        break;
    }
    return;
}

RSAPI void Cson_free(JsonParserValue *node){
    switch(node->type){
        case JSON_BOOLEAN:
        case JSON_DOUBLE:
        case JSON_INT:
        case JSON_NULL:
        case JSON_ERROR:
            return;

        case JSON_STRING:
            RSstr_destroy(&node->val.string);
            break;
        case JSON_ARRAY:
            for(int i = 0; i < RSmvec_getSize(&node->val.array); i++){
                Cson_free(RSmvec_getPointer(&node->val.array, i));
                RSmvec_destroy(&node->val.array);
            }
        case JSON_OBJECT:
            for(int i = 0; i < RSmvec_getSize(&node->val.array); i++){
                JsonKeyValuePair *p = RSmvec_getPointer(&node->val.object, i);
                RSstr_destroy(&p->key);
                Cson_free(&p->value);
                RSmvec_destroy(&node->val.object);
            }
        break;
    }
    return;
}

RSAPI JsonParserValue *Cson_get(JsonParserValue *root, char *path){
    if(!root) return NULL;

    JsonParserValue *node = root;

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
            if(json_arr_index >= RSmvec_getSize(&node->val.array)){
                printf("incorrect array index - out of bounds\n");
                return NULL;
            }
            node = RSmvec_getPointer(&node->val.array, json_arr_index);
            index++;
            // if(path[index] != '.' && path[index] != '[' && path[index] != '\0'){
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
                    // index--;
                    break;
                }
                index++;
            }
            // if(character == '['){
            //     break;
            // }


            // char temp = path[index];
            // path[index] = '\0';
            for(size_t i = 0; i < node->val.object.size; i++){
                JsonKeyValuePair *kp;
                kp = RSmvec_getPointer(&node->val.object, i);
                char *t = kp->key.text;
                for(int i = start; i < index; i++){
                    if(*t == '\0' || path[i] != *t){
                        goto get_next_key;   
                    }
                    t++;
                }
                if(*t == '\0'){
                    node = &kp->value;
                    goto key_found;
                }
                // if (strcmp((path + start), kp->key.text) == 0){
                //     node = &kp->value;
                //     goto key_found;
                // }
                get_next_key:
            }

            printf("incorrect path : \"%s\" key not found\n", path + start);
            return NULL;
            key_found:
            // path[index] = temp;
            break;
        }
    }
    return node;
}

RSAPI JsonParserValue Cson_parseObject(RSStr *data, size_t *start){
    size_t input_size = data->size;
    size_t index = *start;
    JsonParserValue ret_val;
    ret_val.type = JSON_OBJECT;
    ret_val.val.object = CVECTOR_EMPTY(JsonKeyValuePair, NULL, NULL);
    JsonKeyValuePair keyVal_pair;
    char character;
    RSstr_get(data, index, &character);
    if(character == '}'){
        return ret_val;
    }
    while(index < input_size){
        RSstr_get(data, index, &character);
        //I have to check if the object has been closed
        if(character !='\"'){
            //free here
            Cson_free(&ret_val);
            ret_val.type = JSON_ERROR;
            return ret_val;
        }
        index++;
        JsonParserValue temp = Cson_parseString(data, &index);
        if(temp.type == JSON_ERROR){
            //free here
            Cson_free(&ret_val);
            RSstr_destroy(&temp.val.string);
            ret_val.type = JSON_ERROR;
            return ret_val;
        }
        keyVal_pair.key = temp.val.string;
        RSstr_get(data, index, &character);

        if(character != ':'){
            //free here
            Cson_free(&ret_val);
            RSstr_destroy(&temp.val.string);
            ret_val.type = JSON_ERROR;
            return ret_val;
        }
        index++;
        RSstr_get(data, index, &character);



        switch (character) {
        case '\"':
            index++;
            temp = Cson_parseString(data, &index);
            break;
        case '[':
            index++;
            temp = Cson_parseArray(data, &index);
            break;
        case '{':
             index++;
            temp = Cson_parseObject(data, &index);
            break;    
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            temp = Cson_parseNumeric(data, &index);
            break;
        case 't': case 'f':
            temp = Cson_parseBoolean(data, &index);
            break;
        case 'n':
            temp = Cson_parseNull(data, &index);
            break;
        default:
            Cson_free(&ret_val);
            RSstr_destroy(&temp.val.string);
            ret_val.type = JSON_ERROR;
            return ret_val;
            break;
        }

        keyVal_pair.value = temp;
        RSmvec_push(&ret_val.val.object, &keyVal_pair);
        RSstr_get(data, index, &character);
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

RSAPI JsonParserValue Cson_parseArray(RSStr *data, size_t *start){
    size_t index = *start;
    size_t input_size = data->size;
    JsonParserValue ret_val;
    JsonParserValue temp;
    ret_val.type = JSON_ARRAY;
    ret_val.val.object = CVECTOR_EMPTY(JsonParserValue, NULL, NULL);
    char character;
    RSstr_get(data, index, &character);
    // if(character == ']'){
    //     *start = ++index;
    //     return ret_val;
    // }
    if(character == ','){
        ret_val.type = JSON_ERROR;
        return ret_val;
    }

    while(index < input_size){
        RSstr_get(data, index, &character);
        switch (character){
        case ',':
            index++;
            continue;
            break;
        case '\"':
            index++;
            temp = Cson_parseString(data, &index);
            break;
        case '[':
            temp = Cson_parseArray(data, &index);
            break;
        case '{':
            index++;
            temp = Cson_parseObject(data, &index);
            break;
        case 't': case 'f':
            temp = Cson_parseBoolean(data, &index);
            break;
        case 'n':
            temp = Cson_parseNull(data, &index);
            break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            temp = Cson_parseNumeric(data, &index);
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
        RSmvec_push(&ret_val.val.array, &temp);
        // index++;
    }
    return ret_val;
}

RSAPI JsonParserValue Cson_parseString(RSStr *data, size_t *start){
    size_t input_size = data->size;
    size_t index = *start;

    char temp;    
    while(index < input_size){
        char character;
        RSstr_get(data, index, &character);
        switch (character) {
            case '\\':
                index++;
                if(index == input_size){
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

    return (JsonParserValue){.type = JSON_ERROR};

    return_string:
    JsonParserValue ret_val = {
        .type = JSON_STRING,
        .val.string = RSstr_createAssign(data->text + *start)
    };
    data->text[index] = temp;
    *start = index + 1;
    return ret_val;
}

RSAPI JsonParserValue Cson_parseBoolean(RSStr *data, size_t *start){
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

RSAPI JsonParserValue Cson_parseNull(RSStr *data, size_t *start){
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

RSAPI JsonParserValue Cson_parseNumeric(RSStr *data, size_t *start){
    size_t input_size = data->size;
    size_t index = *start;

    char temp;
    int number = 0;
    double decimal = 0;
    char character;
    JsonParserValue ret_val;
    while(index < input_size){
        RSstr_get(data, index, &character);
        
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
    RSstr_get(data, index, &character);
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
        RSstr_get(data, index, &character);
        
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

RSAPI JsonParserValue Cson_loadData(RSStr *data){
    if(!data || !data->size ) return (JsonParserValue){.type = JSON_ERROR};
    RSStr white_spaces = RSstr_createAssign(" \n\t\r");
    RSstr_removeCharacterSet(data, &white_spaces);
    RSstr_destroy(&white_spaces);

    size_t input_size = data->size;
    size_t index = 0;
    JsonParserValue ret_val;
    // while(index < input_size){
        char character;
        RSstr_get(data, index, &character);
        switch (character) {
        case '\"':
            index++;
            ret_val = Cson_parseString(data, &index);
            break;
        case '[':
            index++;
            ret_val = Cson_parseArray(data, &index);
            break;
        case '{':
            index++;
            ret_val = Cson_parseObject(data, &index);
            break;    
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            ret_val = Cson_parseNumeric(data, &index);
            break;
        case 't': case 'f':
            ret_val = Cson_parseBoolean(data, &index);
            break;
        case 'n':
            ret_val = Cson_parseNull(data, &index);
            break;
        default:
            break;
        // }

        // index++;
    }

    return ret_val;
}