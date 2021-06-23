#include "cjson.h"

int main() {
  cjson_null* null = CJSON_NULL_INITIALIZER;
  char buf[10];
  cjson_null_stringify(null, (char*)buf, 10);

  printf("%s\n", buf);

  // cjson_number number = cjson_number_integer(10);
  // int ret = cjson_null_stringify(number, (char*)buf, 10);
  // printf("%d\n", ret);

  int err = 0;
  null = cjson_null_parse("null", 4, &err);
  cjson_null_stringify(null, (char*)buf, 10);
  printf("%s, %d\n", buf, err);
}
