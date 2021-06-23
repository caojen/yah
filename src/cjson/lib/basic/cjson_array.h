#ifndef CJSON_ARRAY_H
#define CJSON_ARRAY_H

#include "cjson_type.h"
#include "cjson_list.h"
#include "cjson_bool.h"
#include "cjson_null.h"
#include "cjson_number.h"
#include "cjson_object.h"
#include "cjson_string.h"

struct __cjson_array {
  TYPE type;
  cjson_list *list;
};

typedef struct __cjson_array cjson_array;

cjson_array* cjson_array_init();

#define CJSON_ARRAY_INITIALIZER cjson_array_init()

/**
 * Push a value at the end of array
 */
int cjson_array_push(cjson_array* array, void* value);

/**
 * Get the value at index. if out of range, return NULL;
 */
void* cjson_array_at(const cjson_array* array, unsigned index);

/**
 * Get the length of array
 */
unsigned cjson_array_length(cjson_array* array);

/**
 * free this array, but not free the items.
 */
int cjson_array_free(cjson_array* array);

/**
 * free this array, including the items.
 */
int cjson_array_free_whole(cjson_array* array);

int cjson_array_stringify(const cjson_array* array, unsigned char* buf, unsigned maxsz);

#endif
