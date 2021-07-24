#include "cjson.h"

int main() {
  cjson_number* one = cjson_number_integer(1);
  cjson_number* two = cjson_number_integer(2);
  cjson_number* three = cjson_number_integer(3);
  cjson_number* done = cjson_number_double(2.09);
  cjson_number* dtwo = cjson_number_double(905.12);
  cjson_number* four = cjson_number_integer(4);

  cjson_array* array = cjson_array_init();
  cjson_array_push(array, one);
  cjson_array_push(array, two);
  cjson_array_push(array, three);
  cjson_array_push(array, done);
  cjson_array_push(array, dtwo);
  cjson_array_push(array, four);
  cjson_string* sone = cjson_string_from_cstring("one");
  cjson_array_push(array, sone);
  cjson_string* stwo = cjson_string_from_cstring("two23\"\\");
  cjson_array_push(array, stwo);
  cjson_string* null = cjson_null_create();
  cjson_array_push(array, null);

  char buf[1000];
  int res;
  res = cjson_array_stringify(array, buf, 1000);

  printf("%s, %d\n", buf, res);
  {
    // array an object
    cjson_object* object = cjson_object_init();

    cjson_string* sone = cjson_string_parse("one");
    cjson_number* none = cjson_number_integer(1);
    cjson_string* stwo = cjson_string_parse("two");
    cjson_number* ntwo = cjson_number_integer(2);
    cjson_string* sthr = cjson_string_parse("three");
    cjson_number* nthr = cjson_number_integer(3);
    cjson_string* sfou = cjson_string_parse("four");
    cjson_number* nfou = cjson_number_integer(4);
    cjson_string* sfiv = cjson_string_parse("five\"\"\"\"\"");
    cjson_string* sfv = cjson_string_parse("ok. it is a string...");
    cjson_string* six = cjson_string_parse("six");
    cjson_null* null_six = CJSON_NULL_INITIALIZER;
    cjson_string* sev = cjson_string_parse("sev");
    cjson_bool* bt = CJSON_BOOL_TRUE;
    cjson_string* eig = cjson_string_parse("eight");
    cjson_bool* bf = CJSON_BOOL_FALSE;
    cjson_bool* bf2 = CJSON_BOOL_FALSE;
    
    cjson_object_set(object, sone, none);
    cjson_object_set(object, stwo, ntwo);
    cjson_object_set(object, sthr, nthr);
    cjson_object_set(object, sfou, nfou);
    cjson_object_set(object, sfiv, sfv);
    cjson_object_set(object, six, null_six);
    cjson_object_set(object, sev, bt);
    cjson_object_set(object, eig, bf);
    
    cjson_array_push(array, object);
  }
  res = cjson_array_stringify(array, buf, 1000);
  printf("%s, %d\n", buf, res);
}