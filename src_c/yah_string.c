#include <string.h>

#include "yah_string.h"
#include "yah_log.h"

int
yah_string_prefix(const char* a, const char* b) {
    if(a == NULL) {
        if(b == NULL) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if(b == NULL) {
            return 1;
        }
    }
    // a | b != NULL
    int alen = strlen(a);
    int blen = strlen(b);
    if(alen < blen) {
        return 0;
    } else if(alen == blen) {
        if(strcmp(a, b) == 0) {
            return 1;
        } else {
            return 0;
        }
    }
    // strlen(a) < strlen(b)
    while(*a == *b) {
        ++a;
        ++b;
    }

    if(*b == 0) {
        return 1;
    } else {
        return 0;
    }
}

int yah_string_cmp(void* __a, unsigned as, void* __b, unsigned bs) {
    if(as != bs) {
        return 1;
    }

    char* a = (char*) __a;
    char* b = (char*) __b;

    for(int i = 0; i < as ;i++) {
        if(a[i] != b[i]) {
            // just return, secure is not concerned.
            return 1;
        }
    }
    return 0;
}

void
yah_string_destory(void* a, unsigned as) {
    free(a);
}

void*
yah_string_copy(void* a, unsigned as) {
    void* r = malloc (as);
    if(r == NULL) {
        yah_error("string copy error, malloc return NULL");
    }
    memcpy(r, a, as);
    return r;
}

char*
yah_string_get_next_part(char* a, char** begin, char** end) {
    while(*a && *a == ' ') {
        ++a;
    }

    if(!a) {
        return NULL;
    }

    *begin = a;
    *end = a;
    while(**end && **end != ' ') {
        ++(*end);
    }
    return a;
}

int
yah_string_substring(const char* a, const char* b) {

    /**
     * The worst implementation of substring.
     * Time = O(ab)
     */
    const char* last_a = a;
    const char* ptr_a = a;
    const char* ptr_b = b;
    int len_a = strlen(a);
    int len_b = strlen(b);

    if(len_a < len_b) {
        return YAH_ISNOT_SUBSTRING;
    } else if(len_a == len_b && strcmp(a, b) == 0) {
        return YAH_IS_SUBSTRING;
    } else {
        while(*ptr_a && *ptr_b) {
            if(*ptr_a == *ptr_b) {
                ++ptr_a, ++ptr_b;
            } else {
                ++last_a;
                ptr_a = last_a;
                ptr_b = b;
            }
        }
        if((!*ptr_a && !*ptr_b) || *ptr_a) {
            return YAH_IS_SUBSTRING;
        } else {
            return YAH_ISNOT_SUBSTRING;
        }
    }
}
