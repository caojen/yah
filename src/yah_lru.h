/**
 * A memory cache using lru.
 */

#ifndef YAH_LRU_H
#define YAH_LRU_H

typedef unsigned long yah_hash_key;

// a hash function
// see http://www.cse.yorku.ca/~oz/hash.html
// modified string into void*
// bytes: the bytes that will be hashed
// bysz: specify the number of bytes
yah_hash_key
hash(void* bytes, unsigned int bysz);

struct yah_cache_node {
    yah_hash_key key;
    void* value;
    unsigned int size;      // specify the size that value takes

    // cmp function for value
    // like strcmp, return 0 if equal
    int (*cmp)(void* v1, unsigned int s1, void* v2, unsigned int s2);

    // the way to destory value
    void (*destory)(void* value, unsigned int size);

    // next and prev different key
    struct yah_cache_node* next;
    struct yah_cache_node* prev;
    // next same key
    struct yah_cache_node* next_same;
};

struct yah_cache_list {
    struct yah_cache_node* head;
    struct yah_cache_node* tail;

    unsigned int count;
};

struct yah_cache {
    // list->head is the currently used node
    struct yah_cache_list* list;
    unsigned int max;

    // the cmp function that use to init node
    int (*cmp)(void* v1, unsigned int s1, void* v2, unsigned int s2);
    // the destory function that use to init node
    void (*destory)(void* value, unsigned int size);
};

// init a cache with max
// the default max = 32 if max = 0
// provide the cmp and destory to init the cache
struct yah_cache* yah_cache_init(unsigned int max,
    int (*cmp)(void* v1, unsigned int s1, void* v2, unsigned int s2),
    void (*destory)(void* value, unsigned int size));

// destory a cache

#endif
