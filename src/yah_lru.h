/**
 * A memory cache using lru.
 */

#ifndef YAH_LRU_H
#define YAH_LRU_H

#include <pthread.h>

typedef unsigned long yah_hash_key;

#define YAH_CACHE_NODE_EXISTS 1
#define YAH_CACHE_NODE_NOTEXISTS 0

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

    time_t time;      // the time that create or update this node

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
    // copy a new value from the old value with size sz
    void* (*copy)(void* old, unsigned sz);
    // mutex to access the list
    pthread_mutex_t mutex;
    // out-of-date second count. 0 means never out-of-data
    unsigned odtime;
};

// init a cache with max
// the default max = 32 if max <= 10
// provide the cmp and destory to init the cache
struct yah_cache* yah_cache_init(unsigned int max, unsigned odtime,
    int (*cmp)(void* v1, unsigned int s1, void* v2, unsigned int s2),
    void (*destory)(void* value, unsigned int size),
    void* (*copy)(void* old, unsigned sz));

// destory a cache
// if success, return 0
int yah_cache_destory(struct yah_cache* cache);

// update a cache with value
// returns:
// if the value already exists (before update), return YAH_CACHE_NODE_EXISTS
// else return YAH_CACHE_NODE_NOTEXISTS
// no matter how, the cache will be update
//
// multi-thread warning:
// this function may block because the mutex
int yah_cache_update(struct yah_cache* cache, void* value, unsigned sz);

#endif
