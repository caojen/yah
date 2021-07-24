#ifndef CJSON_OBJECT_H
#define CJSON_OBJECT_H

#include "cjson_type.h"
#include "cjson_string.h"
#include "cjson_list.h"

struct __cjson_string;    // defined: cjson_string.h
typedef struct __cjson_string cjson_string;

struct __cjson_object_item {
  cjson_string* key;
  void* value;
};

typedef struct __cjson_object_item cjson_object_item;

cjson_object_item* cjson_object_item_create(cjson_string* key, void* value);

int cjson_object_item_free(cjson_object_item* item);

struct __cjson_object {
  TYPE type;
  cjson_list* list;
};

typedef struct __cjson_object cjson_object;

cjson_object* cjson_object_init();

/**
 * set a key-value
 * 
 * If the key not exists, add it to list. use the key directly.
 * If the key exists, set it and free the old value.
 * 
 * use the value directly.
 */
int cjson_object_set(cjson_object* object, cjson_string* key, void* value);

/**
 * delete the key if exists.
 * will free that value and the origin key.
 * 
 * will not free the param key
 */
int cjson_object_del(cjson_object* object, cjson_string* key);

int cjson_object_stringify(cjson_object* object, unsigned char* buf, unsigned maxsz);

// free this object, but not free the item
int cjson_object_free(cjson_object* object);

// free this object, including the items
int cjson_object_free_whole(cjson_object* object);

#endif
