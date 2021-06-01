/**
 * some helper funtions to deal with C-string
 */

#ifndef YAH_STRING_H
#define YAH_STRING_H

// if b is a's prefix, return 1
// else return 0
// treat '' as null
// if a is null:
//      if b is null: return 1
//      if b is not null, return 0
// if a is not null:
//      if b is null, return 1
//      if b is not null, compare them
int yah_string_prefix(const char* a, const char* b);

#endif
