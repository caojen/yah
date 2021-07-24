#ifndef CJSON_BOOL_H
#define CJSON_BOOL_H

#include "cjson_type.h"

struct __cjson_bool {
  TYPE type;
  int istrue;
};

typedef struct __cjson_bool cjson_bool;

cjson_bool* cjson_bool_create(int istrue);

#define CJSON_BOOL_TRUE cjson_bool_create(1)
#define CJSON_BOOL_FALSE cjson_bool_create(0)

int cjson_bool_stringify(cjson_bool* _bool, unsigned char* buf, unsigned maxsz);

cjson_bool* cjson_bool_parse(unsigned char* buf, unsigned size, int* err);

int cjson_bool_not(cjson_bool* _bool);
int cjson_bool_set(cjson_bool* _bool, int istrue);

int cjson_bool_free(cjson_bool* _bool);

#endif
