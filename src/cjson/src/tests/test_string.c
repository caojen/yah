#include "cjson.h"

int main() {
  {
    cjson_string* s = cjson_string_parse("10");
    char buf[20];
    cjson_string_stringify(s, buf, 20);
    printf("%s\n", buf);

    s = cjson_string_parse("sk99");
    cjson_string_stringify(s, buf, 20);
    printf("%s\n", buf);

    s = cjson_string_parse("this's abc \"ttt\"");
    cjson_string_stringify(s, buf, 20);
    printf("%s\n", buf);

    s = cjson_string_parse("what? \\/");
    cjson_string_stringify(s, buf, 20);
    printf("%s\n", buf);
  }
  char buf[20];
  cjson_string* s = cjson_string_parse("10");
  cjson_string_stringify(s, buf, 20);
  printf("%s\n", buf);
  cjson_string_set_cstring(s, "1");
  cjson_string_stringify(s, buf, 20);
  printf("%s\n", buf);
  cjson_string_set_cstring(s, "abcde");
  cjson_string_stringify(s, buf, 20);
  printf("%s\n", buf);
  cjson_string_set_cstring(s, "999");
  cjson_string_stringify(s, buf, 20);
  printf("%s\n", buf);
  cjson_string_set_cstring(s, "sdsds");
  cjson_string_stringify(s, buf, 20);
  printf("%s\n", buf);
  cjson_string_set_cstring(s, "");
  cjson_string_stringify(s, buf, 20);
  printf("%s\n", buf);

}