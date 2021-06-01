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

int yah_string_cmp(char* a, unsigned as, char* b, unsigned bs) {
    if(as != bs) {
        return 1;
    }

    for(int i = 0; i < as ;i++) {
        if(a[i] != b[i]) {
            // just return, secure is not concerned.
            return 1;
        }
    }
    return 0;
}
