#include <stdlib.h>

#include "yah_mem.h"

void* yah_mem_alloc(unsigned size) {
    if(size == 0) {
        return NULL;
    }
    
    void* r = malloc(size);

    return r;
}

void yah_mem_free(void* ptr) {
    free(ptr);
}