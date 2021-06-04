#include <stdio.h>
#include "yah_json.h"

int main() {
    Json json;

    Value v;
    yah_json_value_integer(30, &v);
    yah_json_set(&json, "abcdef", &v);
    yah_json_value_integer(100, &v);
    yah_json_set(&json, "deee", &v);
    yah_json_value_string("qwertyii", &v);
    yah_json_set(&json, "v", &v);
    yah_json_value_string("qwert\"\"''yii", &v);
    yah_json_set(&json, "v\"d", &v);

    char output[YAH_JSON_SERIALIZE_LENGTH];
    yah_json_serialize(&json, output);
    printf("%s\n", output);
}
