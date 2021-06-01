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
    yah_log("compare as = %d, bs = %d", as, bs);
    if(as != bs) {
        return 1;
    }

    char* a = (char*) __a;
    char* b = (char*) __b;

    yah_log_hex(a, as);
    yah_log_hex(b, bs);

    for(int i = 0; i < as ;i++) {
        if(a[i] != b[i]) {
            // just return, secure is not concerned.
            yah_log("first diff at : %d", i);
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
