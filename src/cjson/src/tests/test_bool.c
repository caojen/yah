#include "cjson_bool.h"

int main() {
  cjson_bool* b = CJSON_BOOL_TRUE;
  char buf[20] = { 0 };
  cjson_bool_stringify(b, (char*)buf, 20);
  printf("%s\n", buf);
  b = CJSON_BOOL_FALSE;
  cjson_bool_stringify(b, (char*)buf, 20);
  printf("%s\n", buf);
  int err;
  b = cjson_bool_parse("true", 4, &err);
  cjson_bool_stringify(b, buf, 20);
  printf("%s, %d\n", buf, err);
  b = cjson_bool_parse("True", 4, &err);
  cjson_bool_stringify(b, buf, 20);
  printf("%s, %d\n", buf, err);
  b = cjson_bool_parse("false", 5, &err);
  cjson_bool_stringify(b, buf, 20);
  printf("%s, %d\n", buf, err);
  b = cjson_bool_parse("False", 5, &err);
  cjson_bool_stringify(b, buf, 20);
  printf("%s, %d\n", buf, err);
}
