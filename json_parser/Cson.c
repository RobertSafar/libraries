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

CSAPI JsonParserValue Cson_loadData(String *data){
    if(!data || !data->size ) return (JsonParserValue){.type = JSON_ERROR};
    String white_spaces = String_createAssign(" \n\t\r");
    String_removeCharacterSet(data, &white_spaces);
    String_destroy(&white_spaces);

    size_t input_size = data->size;
    size_t index = 0;
    JsonParserValue ret_val;
    while(index < input_size){
        char character;
        String_get(data, index, &character);
        switch (character) {
        case '\"':
            index++;
            ret_val = Cson_getString(data, &index);
            break;
        case '[':
            
            break;
        case '{':
            
            break;    
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            
            break;
        case 't': case 'f':
            
            break;
        case 'n':
            break;
        default:
            break;
        }

        // index++;
    }

    return ret_val;
}