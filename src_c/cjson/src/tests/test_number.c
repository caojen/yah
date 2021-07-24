#include <stdio.h>
#include "cjson.h"

int main() {
  cjson_number* x = cjson_number_integer(1012);
  char buf[20] = { 0 };
  cjson_number_stringify(x, (char*)buf, 20);
  printf("%s\n", buf);

  cjson_number* y = cjson_number_double(172.32);
  cjson_number_stringify(y, (char*)buf, 20);
  printf("%s\n", buf);

  int err = 0;
  cjson_number* p = cjson_number_parse("10", 2, &err);
  cjson_number_stringify(p, (char*)buf, 20);
  printf("%s\n", buf);
  p = cjson_number_parse("10.123", 6, &err);
  cjson_number_stringify(p, (char*)buf, 20);
  printf("%s\n", buf);
  p = cjson_number_parse("0.2", 3, &err);
  cjson_number_stringify(p, (char*)buf, 20);
  printf("%s\n", buf);
  p = cjson_number_parse("102.", 4, &err);
  cjson_number_stringify(p, (char*)buf, 20);
  printf("%s\n", buf);
  p = cjson_number_parse(".928", 4, &err);
  cjson_number_stringify(p, (char*)buf, 20);
  printf("%s\n", buf);
}
