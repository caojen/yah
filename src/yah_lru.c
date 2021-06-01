#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "yah_lru.h"
#include "yah_log.h"

yah_hash_key
hash(void* bytes, unsigned int bysz) {
    unsigned char* str = (unsigned char*) bytes;
    yah_hash_key hash = 5381;
    for(unsigned int i = 0; i < bysz; i++) {
        hash = ((hash << 5) + hash) + str[i];
    }
    return hash;
}

struct yah_cache*
yah_cache_init(unsigned int max, unsigned odtime,
    int (*cmp)(void* v1, unsigned int s1, void* v2, unsigned int s2),
    void (*destory)(void* value, unsigned int size),
    void* (*copy)(void* old, unsigned sz)) {
    
    if(max <= 10) {
        max = 32;
    }
    struct yah_cache* cache = 
        (struct yah_cache*) malloc (sizeof(struct yah_cache));

    cache->cmp = cmp;
    cache->destory = destory;
    cache->copy = copy;
    cache->max = max;
    cache->odtime = odtime;

    // init the mutex
    if(pthread_mutex_init(&cache->mutex, NULL) != 0) {
        yah_quit("lru: cannot init mutex");
    }

    // init the list
    cache->list =
        (struct yah_cache_list*) malloc (sizeof(struct yah_cache_list));
    memset(cache->list, 0, sizeof(struct yah_cache_list));
    return cache;
}

int
yah_cache_destory(struct yah_cache* cache) {
    if(cache != NULL) {
        struct yah_cache_list* list = cache->list;
        struct yah_cache_node* node = list->head;
        struct yah_cache_node* next;
        struct yah_cache_node* next_same_key_node;
        // loop for each node
        while(node) {
            next = node->next;
            while(node) {
                next_same_key_node = node->next_same;
                node->destory(node->value, node->size);
                node = next_same_key_node;
            }
            node = next;
        }
        // all nodes are destoried, delete the list
        free(cache->list);
        // list is destoried, delete the cache
        free(cache);
    }

    return 0;
}

int yah_cache_update(struct yah_cache* cache, void* value, unsigned size) {
    yah_log("lru: updatding...");
    yah_log_hex(value, size);

    int ret = YAH_CACHE_NODE_NOTEXISTS;

    // get the hash key
    yah_hash_key key = hash(value, size);
    yah_log("lru: hash result = %lu", key);
    // get the mutex:
    pthread_mutex_lock(&cache->mutex);
    yah_log("lru: mutex got. current size = %u", cache->list->count);
    // find if there is a key exists
    struct yah_cache_list* list = cache->list;
    struct yah_cache_node* node = list->head;
    yah_log("lru: 1");
    while(node) {
        yah_log("current key = %lu", node->key);
        if(node->key == key) {
            yah_log("key is same");
            // find the key. test its value:
            struct yah_cache_node* ptr = node;
            yah_log("checking ptrs");
            while(ptr) {
                yah_log("checkint ptr->key = %lu value if is equaled", ptr->key);
                if(ptr->cmp(ptr->value, ptr->size, value, size) == 0) {
                    yah_log("ok. it is equal");
                    // find the same value
                    if(cache->odtime != 0) {
                        yah_log("lru: checking key if out-of-time");
                        // check the time
                        time_t now = time(NULL);
                        if(now - ptr->time >= cache->odtime) {
                            yah_log("lru: out of time, now = %lu, ptr->time = %lu, odtime = %d", now, ptr->time, cache->odtime);
                            // out of date
                            // treat that the node is not exists
                            // update the time, reorder the node at the head of cache->list
                            ptr->time = now;
                            if(node != list->head) {
                                if(node == list->tail) {
                                    node->prev->next = NULL;
                                    node->prev = NULL;
                                    node->next = list->head;
                                    list->head->prev = list->head;
                                } else {
                                    struct yah_cache_node* prev = node->prev;
                                    struct yah_cache_node* next = node->next;
                                    prev->next = next;
                                    next->prev = prev;
                                    node->next = list->head;
                                    node->prev = NULL;
                                    list->head->prev = node;
                                    list->head = node;
                                }
                            }
                            yah_log("lru: key out-of-time and update, finish");
                            // finish:
                            goto finish;
                        }
                    }
                    yah_log("lru: key found");
                    ret = YAH_CACHE_NODE_EXISTS;
                    goto findend;
                } else {
                    yah_log("lru: config. cmp return false");
                }
                ptr = ptr->next_same;
            }
            goto findend;
        }
        node = node->next;
    }
    yah_log("lru: find ret = %d", ret);
findend:
    if(ret == YAH_CACHE_NODE_EXISTS) {
        yah_log("lru: reorder node into head");
        if(node != list->head) {
            // move the node at the head of list
            if(node == list->tail) {
                // move the tail to head
                struct yah_cache_node* prev = list->tail->prev;
                prev->next = NULL;
                list->tail->prev = NULL;
                list->tail->next = list->head;
                list->head->prev = list->tail;
                list->head = list->tail;
                list->tail = prev;
            } else {
                struct yah_cache_node* prev = node->prev;
                struct yah_cache_node* next = node->next;
                prev->next = next;
                next->prev = prev;
                node->next = list->head;
                node->prev = NULL;
                list->head->prev = node;
                list->head = node;
            }
        }
    } else {
        yah_log("lru: insert into list");
        // the value is not exists. add to cache
        // get the new copy of value
        void* newvalue = cache->copy(value, size);
        struct yah_cache_node* newnode = 
            (struct yah_cache_node*) malloc (sizeof(struct yah_cache_node));
        memset(newnode, 0, sizeof(struct yah_cache_node));
        newnode->key = key;
        newnode->value = newvalue;
        newnode->size = size;
        newnode->cmp = cache->cmp;
        newnode->destory = cache->destory;
        newnode->time = time(NULL);

        if(list->head == NULL) {
            yah_log("lru: the list is empty , just insert into head");
            // empty, just insert it
            list->head = list->tail = newnode;  
            list->count++;  
        } else {
            yah_log("lru: the list is not empty");
            if(list->count < cache->max) {
                yah_log("lru: not exceed");
                if(node && key == node->key) {
                    yah_log("lru: key exists, append it");
                    // just insert into the end of node
                    struct yah_cache_node* ptr = node;
                    while(ptr->next_same != NULL) {
                        ptr = ptr->next_same;
                    }
                    ptr->next = newnode;
                } else {
                    yah_log("lru: key not exists, add to head");
                    // insert into the head of list
                    newnode->next = list->head;
                    list->head->prev = newnode;
                    list->head = newnode;
                }
                list->count++;
            } else {
                yah_log("lru: exceed, remove tail");
                // remove the tail
                // decrease count
                yah_log("tail at: %p", list->tail);
                struct yah_cache_node* tail = list->tail;
                yah_log("prev of tail at %p", list->tail->prev);
                struct yah_cache_node* prev = tail->prev;
                prev->next = NULL;

                struct yah_cache_node* next;
                yah_log("lru: removing all links");
                while(tail) {
                    next = tail->next_same;
                    tail->destory(tail->value, tail->size);
                    free(tail);
                    tail = next;
                    list->count--;
                }

                yah_log("lru: add to head");

                // add new node to head
                list->head->prev = newnode;
                newnode->next = list->head;
                list->head = newnode;
            }
        }
    }
finish:
    yah_log("unlock mutex, final size = %d", list->count);
    yah_log("keylist:");
    struct yah_cache_node* j = cache->list->head;
    while(j) {
        yah_log("%lu", j->key);
        j = j->next;
    }
    pthread_mutex_unlock(&cache->mutex);
    return ret;
}
