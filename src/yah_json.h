#ifndef YAH_JSON_H
#define YAH_JSON_H

#include "yah_core.h"

#define YAH_JSON_MAX_KEY_COUNT      10
#define YAH_JSON_MAX_KEY_LENGTH     64
#define YAH_JSON_MAX_VALUE_LENGTH   YAH_CAPTURE_LINE
#define YAH_JSON_SERIALIZE_LENGTH ((YAH_JSON_MAX_KEY_COUNT + 1) * (YAH_JSON_MAX_KEY_LENGTH + YAH_JSON_MAX_VALUE_LENGTH))

struct yah_json_value {
    enum {
        integer = 0,
        string
    } type;

    union {
        int integer;
        char string[YAH_JSON_MAX_VALUE_LENGTH];
    } value;
};

int yah_json_value_integer(int integer, struct yah_json_value* output);
int yah_json_value_string(const char* string, struct yah_json_value* output);

struct yah_json {
    unsigned count;
    char keys[YAH_JSON_MAX_KEY_COUNT][YAH_JSON_MAX_KEY_LENGTH];
    struct yah_json_value values[YAH_JSON_MAX_KEY_COUNT];
};

typedef struct yah_json_value Value;

typedef struct yah_json Json;

int yah_json_set(Json* json, const char* key, const struct yah_json_value* value);
int yah_json_serialize(const Json* json, char output[YAH_JSON_SERIALIZE_LENGTH]);

#endif
