#ifndef CJSON_NULL_H
#define CJSON_NULL_H

#include "cjson_type.h"

struct __cjson_null {
  TYPE type;
};

typedef struct __cjson_null cjson_null;

cjson_null* cjson_null_create();

#define CJSON_NULL_INITIALIZER cjson_null_create()

int cjson_null_stringify(cjson_null* null, unsigned char* buf, unsigned maxsz);

/**
 * try parse a string into null
 * 
 * i.e, "null"(size = 4) => cjson_null
 */
cjson_null* cjson_null_parse(unsigned char* buf, unsigned size, int* err);

int cjson_null_free(cjson_null* null);

#endif
