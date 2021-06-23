#include "cjson_null.h"

cjson_null*
cjson_null_create() {
  cjson_null* ret = (cjson_null*) malloc (sizeof(cjson_null));
  ret->type = NUL;

  return ret;
}

int
cjson_null_stringify(cjson_null* null, unsigned char* buf, unsigned maxsz) {
  if(maxsz < 5) {
    return CJSON_NOSPACE;
  } else if(buf == NULL) {
    return CJSON_BUFCLOSE;
  } else if(null == NULL) {
    return CJSON_UNINIT;
  } else if(cjson_get_type(null) != NUL) {
    return CJSON_ERRTYPE;
  }

  buf[0] = 'n';
  buf[1] = 'u';
  buf[2] = 'l';
  buf[3] = 'l';
  buf[4] = 0;

  return CJSON_OK;
}

cjson_null*
cjson_null_parse(unsigned char* buf, unsigned size, int* err) {
  cjson_null* ret = CJSON_NULL_INITIALIZER;
  if(size != 4) {
    *err = CJSON_NNUL;
  } else if(buf == NULL) {
    *err = CJSON_BUFCLOSE;
  } else {
    if((buf[0] == 'n' || buf[0] == 'N') &&
       (buf[1] == 'u' || buf[1] == 'U') &&
       (buf[2] == 'l' || buf[2] == 'L') &&
       (buf[3] == 'l' || buf[3] == 'L')) {
      *err = CJSON_OK;
    } else {
      *err = CJSON_NNUL;
    }
  }

  return ret;
}

int
cjson_null_free(cjson_null* null) {
  if(null) {
    free(null);
  }
  return CJSON_OK;
}
