#include "cjson_bool.h"

cjson_bool*
cjson_bool_create(int istrue) {
  cjson_bool* ret = (cjson_bool*) malloc (sizeof(cjson_bool));
  memset(ret, 0, sizeof(cjson_bool));

  ret->type = BOOLEAN;
  if(istrue) {
    ret->istrue = 1;
  } else {
    ret->istrue = 0;
  }

  return ret;
}

int
cjson_bool_stringify(cjson_bool* _bool, unsigned char* buf, unsigned maxsz) {
  if(_bool == NULL) {
    return CJSON_UNINIT;
  } else if(buf == NULL) {
    return CJSON_BUFCLOSE;
  } else if(cjson_get_type(_bool) != BOOLEAN) {
    return CJSON_ERRTYPE;
  }

  if(_bool->istrue) {
    if(maxsz < 5) {
      return CJSON_NOSPACE;
    }
    buf[0] = 't';
    buf[1] = 'r';
    buf[2] = 'u';
    buf[3] = 'e';
    buf[4] = 0;
  } else {
    if(maxsz < 6) {
      return CJSON_NOSPACE;
    }
    buf[0] = 'f';
    buf[1] = 'a';
    buf[2] = 'l';
    buf[3] = 's';
    buf[4] = 'e';
    buf[5] = 0;
  }
  return CJSON_OK;
}

cjson_bool*
cjson_bool_parse(unsigned char* buf, unsigned size, int* err) {
  cjson_bool* ret = CJSON_BOOL_TRUE;
  *err = CJSON_OK;
  if(size != 4 && size != 5) {
    *err = CJSON_NBOOL;
    return ret;
  }

  if(size == 4) {
    // true
    if(CJSON_CHAR_EQUAL_INSENSE(buf[0], 't')) {
      ret = CJSON_BOOL_TRUE;
    } else {
      *err = CJSON_NBOOL;
    }
  } else {
    // false
    if(CJSON_CHAR_EQUAL_INSENSE(buf[0], 'f')) {
      ret = CJSON_BOOL_FALSE;
    } else {
      *err = CJSON_NBOOL;
    }
  }

  return ret;
}

int cjson_bool_not(cjson_bool* _bool) {
  if(_bool == NULL) {
    return CJSON_UNINIT;
  } else if(cjson_get_type(_bool) != BOOLEAN) {
    return CJSON_ERRTYPE;
  }

  if(_bool->istrue) {
    _bool->istrue = 0;
  } else {
    _bool->istrue = 1;
  }
  return CJSON_OK;
}

int cjson_bool_set(cjson_bool* _bool, int istrue) {
  if(_bool == NULL) {
    return CJSON_UNINIT;
  } else if(cjson_get_type(_bool) != BOOLEAN) {
    return CJSON_ERRTYPE;
  }

  if(istrue) {
    _bool->istrue = 1;
  } else {
    _bool->istrue = 0;
  }
  return CJSON_OK;
}

int
cjson_bool_free(cjson_bool* _bool) {
  if(_bool) {
    free(_bool);
  }
  return CJSON_OK;
}