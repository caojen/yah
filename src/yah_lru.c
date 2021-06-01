#include "yah_lru.h"

yah_hash_key
hash(void* bytes, unsigned int bysz) {
    unsigned char* str = (unsigned char*) bytes;
    yah_hash_key hash = 5381;
    for(unsigned int i = 0; i < bysz; i++) {
        hash = ((hash << 5) + hash) + str[i];
    }
    return hash;
}
