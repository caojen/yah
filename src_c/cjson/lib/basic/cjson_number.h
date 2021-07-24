/**
 * number: long long(4/8 bits) or double(8 bits)
 * 
 * number is stored as static always.
 */

#ifndef CJSON_NUMBER_H
#define CJSON_NUMBER_H

#include "cjson_type.h"

typedef long long isize_64;

struct __cjson_number {
  TYPE type;
  enum {
    integer,
    ldouble
  } valuetype;

  union {
    isize_64 i;
    double d;
  } value;
};

typedef struct __cjson_number cjson_number;

/**
 * Construct a cjson_number.
 * A number is an integer(long long) or a double.
 */
cjson_number* cjson_number_integer(isize_64 value);
cjson_number* cjson_number_double(double value);

int cjson_number_free(cjson_number* number);

/**
 * Set a cjson_number
 * If number is null, return CJSON_UNINIT
 * return CJSON_OK as success
 */
int cjson_number_set_integer(cjson_number* number, isize_64 value);
int cjson_number_set_double(cjson_number* number, double value);

/**
 * Specify number is an integer or a double
 */
#define cjson_number_type(number) ((number).valuetype)
#define cjson_number_get_integer(number) ((number).value.i)

#define cjson_number_get_double(number) ((number).value.d)
/**
 * stringify
 * 
 * parse number into string.
 * 
 * For example, 10 => "10", 3.14 => "3.14". Just like other c-string, \0 is appended.
 * 
 * Store the result into buf, maxsz is the buffer max size
 * If success, return CJSON_OK
 * If number is null, return CJSON_UNINIT
 * If buf is null, return CJSON_BUFCLOSE
 * If buf is not enough(the size of buf is specified by maxsz), return CJSON_NOSPACE
 * If number is not type number, return CJSON_ERRTYPE
 */
int cjson_number_stringify(cjson_number* number, unsigned char* buf, unsigned int maxsz);

int cjson_number_stringify_double(cjson_number* number, unsigned char* buf, unsigned int maxsz);
int cjson_number_stringify_integer(cjson_number* number, unsigned char* buf, unsigned int maxsz);

/**
 * parse
 * 
 * parse string into number, specify the length of string
 * 
 * return the pass result
 * 
 * For example, "10"(with size = 2) => 10, "3.14"(with size = 4) => 3.14
 * 
 * If parse error, e.g, "3.14.13", or "3.14a", CJSON_NAN will be returned.
 * 
 * The error code is returned at int* err. If no error, return CJSON_OK
 */
cjson_number* cjson_number_parse(unsigned char* buf, unsigned size, int* err);

/**
 * Define max digit of double
 */
#define CJSON_NUMBER_DOUBLE_MAX_DIGIT 6
#endif
