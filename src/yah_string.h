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

// compare two string, as, bs is the bytes that the string take('\0' included)
// if the string is equaled, return 0
int yah_string_cmp(void* a, unsigned as, void* b, unsigned bs);

// destory a string init with malloc
void yah_string_destory(void* a, unsigned as);

// copy a string with size as - 1
void* yah_string_copy(void* a, unsigned as);

// return the part^{th} of a split with ' '
// treat multi-' ' as one ' '
// return the pointer to **begin
char* yah_string_get_next_part(char* a, char** begin, char** end);

#endif
