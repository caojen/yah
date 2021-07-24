#include <stdlib.h>

#include "cjson_string.h"

cjson_string*
cjson_string_init() {
  cjson_string* ret = (cjson_string*) malloc (sizeof(cjson_string));
  memset(ret, 0, sizeof(cjson_string));

  ret->type = STRING;
  ret->length = ret->capcity = 0;
  ret->buf = NULL;
  return ret;
}

cjson_string* cjson_string_from_cstring(const unsigned char* str) {
  cjson_string* ret = cjson_string_init();
  if(str == NULL || *str == 0) {
    return ret;
  } else {
    ret->length = CJSON_BUFLEN(str);
    ret->capcity = ret->length;
    ret->buf = (unsigned char*) malloc (sizeof(unsigned char) * ret->capcity);
    memcpy(ret->buf, str, ret->capcity * sizeof(unsigned char));
  }
  return ret;
}

int cjson_string_set_cstring(cjson_string* string, const unsigned char* str) {
  if(string == NULL) {
    return CJSON_UNINIT;
  }
  if(str == NULL || *str == 0) {
    string->length = 0;
  }
  unsigned length = CJSON_BUFLEN(str);
  if(string->capcity < length) {
    unsigned char* nptr = (unsigned char*) realloc(string->buf, sizeof(unsigned char) * length);
    if(nptr == NULL) {
      return CJSON_EMEM;
    } else {
      string->buf = nptr;
      string->capcity = length;
      string->length = length;
      memcpy(string->buf, str, sizeof(unsigned char) * length);
      return CJSON_OK;
    }
  } else {
    string->length = length;
    memcpy(string->buf, str, sizeof(unsigned char) * length);
    return CJSON_OK;
  }
}

int
cjson_string_to_cstring(const cjson_string* string, unsigned char* buf, unsigned maxsz) {
  if(maxsz <= string->length) {
    return CJSON_NOSPACE;
  } else if(string == NULL) {
    return CJSON_UNINIT;
  } else if(buf == NULL) {
    return CJSON_BUFCLOSE;
  }
  memcpy(buf, string->buf, sizeof(unsigned char) * string->length);
  buf[string->length + 1] = 0;
  return CJSON_OK;
}

int
cjson_string_free(cjson_string* string) {
  string->length = string->capcity = 0;
  if(string->buf) {
    free(string->buf);
    string->buf = NULL;
  }
  return CJSON_OK;
}

int
cjson_string_stringify(const cjson_string* string, unsigned char* buf, unsigned maxsz) {
  unsigned char* ptr = buf;
  // prefix "
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = '"'; ++ptr;

  for(unsigned i = 0; i < string->length; i++) {
    if(string->buf[i] == '"') {
      CHECK_NO_SPACE(ptr - buf >= maxsz);
      *ptr = '\\'; ++ptr;
      CHECK_NO_SPACE(ptr - buf >= maxsz);
      *ptr = '"'; ++ptr;
    } else if(string->buf[i] == '\\') {
      CHECK_NO_SPACE(ptr - buf >= maxsz);
      *ptr = '\\'; ++ptr;
      CHECK_NO_SPACE(ptr - buf >= maxsz);
      *ptr = '\\'; ++ptr;
    } else {
      CHECK_NO_SPACE(ptr - buf >= maxsz);
      *ptr = string->buf[i]; ++ptr;
    }
  }
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = '"'; ++ptr;
  CHECK_NO_SPACE(ptr - buf >= maxsz);
  *ptr = 0;
  return CJSON_OK;
}

int cjson_string_cmp(cjson_string* a, cjson_string* b) {
  if(a == NULL && b == NULL) {
    return 0;
  } else if(a == NULL || b == NULL) {
    return 1;
  }

  if(a->length != b->length) {
    return 1;
  }
  for(int i = 0; i < a->length; i++) {
    if(a->buf[i] != b->buf[i]) {
      return 1;
    }
  }
  return 0;
}
