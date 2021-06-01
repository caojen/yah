#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "yah_mem.h"
#include "yah_log.h"

void* yah_mem_alloc(unsigned size) {
    if(size == 0) {
        return NULL;
    }

    void* r = malloc(size);
    if(r == NULL) {
        yah_warn("malloc error, %s", strerror(errno));
    }

    return r;
}

void yah_mem_free(void* ptr) {
    free(ptr);
}
