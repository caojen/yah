#include <stdio.h>
#include <stdlib.h>

#include "yah_string.h"

int main() {
    int res;
    res = yah_string_substring("abcd", "a");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "ab");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "abc");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "ac");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "ad");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "dca");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "c");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "cd");
    printf("res = %d\n", res);
    res = yah_string_substring("abcd", "bcd");
    printf("res = %d\n", res);
    exit(0);
}