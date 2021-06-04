#include <string.h>
#include <stdio.h>

#include "yah_json.h"

int
yah_json_set(Json* json, const char* key, const struct yah_json_value* value) {
    if(json == NULL || json->count >= YAH_JSON_MAX_KEY_COUNT) {
        return 1;
    }

    unsigned i = json->count;
    json->count++;

    // store key
    char* key_ptr = json->keys[i];
    const char* ptr = key;
    while(*ptr) {
        if(*ptr == '\"') {
            *key_ptr = '\\';
            ++key_ptr;
        }
        *key_ptr = *ptr;
        ++key_ptr;
        ++ptr;
    }

    // specify the type of value
    switch (value->type) {
        case integer:
            // just memcpy:
            memcpy(&json->values[i], value, sizeof(struct yah_json_value));
            break;
        case string:
            json->values[i].type = string;
            char* value_ptr = json->values[i].value.string;
            ptr = value->value.string;
            while(*ptr) {
                if(*ptr == '"') {
                    *value_ptr = '\\';
                    ++value_ptr;
                }
                *value_ptr = *ptr;
                ++ptr, ++value_ptr;
            }
            break;
    }
    return 0;
}

int
yah_json_serialize(const Json* json, char output[YAH_JSON_SERIALIZE_LENGTH]) {
    if(json == NULL || json->count == 0) {
        sprintf(output, "{}");
        return 0;
    }
    char tmp[YAH_JSON_SERIALIZE_LENGTH] = { 0 };
    int first = 1;

    output[0] = '{';

    for(int i = 0; i < json->count; i++) {
        if(json->values[i].type == string) {
            sprintf(tmp, "\"%s\":\"%s\"", json->keys[i], json->values[i].value.string);
        } else {
            sprintf(tmp, "\"%s\":%d", json->keys[i], json->values[i].value.integer);
        }
        if(first == 1) {
            first = 0;
        } else {
            strcat(output, ",");
        }
        strcat(output, tmp);
    }
    strcat(output, "}");
    return 0;
}

int
yah_json_value_integer(int integer, struct yah_json_value* output) {
    output->type = integer;
    output->value.integer = integer;
    return 0;
}

int
yah_json_value_string(const char* str, struct yah_json_value* output) {
    output->type = string;
    output->value.string[0] = 0;
    strcpy(output->value.string, str);
    return 0;
}
