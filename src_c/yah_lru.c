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
    
    if(max <= 1) {
        max = 2;
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
    // yah_log("lru cache updating");
    int ret = YAH_CACHE_NODE_NOTEXISTS;

    // get the hash key
    yah_hash_key key = hash(value, size);
    // get the mutex:
    pthread_mutex_lock(&cache->mutex);
    // find if there is a key exists
    struct yah_cache_list* list = cache->list;
    struct yah_cache_node* node = list->head;
    while(node) {
        if(node->key == key) {
            // find the key. test its value:
            struct yah_cache_node* ptr = node;
            while(ptr) {
                if(ptr->cmp(ptr->value, ptr->size, value, size) == 0) {
                    // find the same value
                    if(cache->odtime != 0) {
                        // check the time
                        time_t now = time(NULL);
                        if(now - ptr->time >= cache->odtime) {
                            yah_log("lru: %s out of time, key = %lu", value, key);
                            // out of date
                            // treat that the node is not exists
                            // update the time, reorder the node at the head of cache->list
                            ptr->time = now;
                            if(node != list->head) {
                                if(node == list->tail) {
                                    list->tail = node->prev;
                                    node->prev->next = NULL;
                                    node->prev = NULL;
                                    node->next = list->head;
                                    list->head->prev = node;
                                    list->head = node;
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
                            // finish:
                            goto finish;
                        }
                    }
                    ret = YAH_CACHE_NODE_EXISTS;
                    goto findend;
                } else {
                    // yah_log("lru: config. cmp return false");
                }
                ptr = ptr->next_same;
            }
            if(list->count >= cache->max) {
                // remove the tail
                struct yah_cache_node* tail = list->tail;
                if(list->tail == list->head) {
                    list->tail = list->head = NULL;
                } else {
                    tail->prev->next = NULL;
                    list->tail = tail->prev;
                }
                struct yah_cache_node* ptrs;
                while(tail) {
                    ptrs = tail->next_same;
                    tail->destory(tail->value, tail->size);
                    free(tail);
                    tail = ptrs;
                    list->count--;
                }
                // set the tail to push the newnode at the head of the list
                if(node == tail) {
                    node = NULL;
                }
            }
            goto findend;
        }
        node = node->next;
    }
findend:
    if(ret == YAH_CACHE_NODE_EXISTS) {
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
            // empty, just insert it
            list->head = list->tail = newnode;  
            list->count++;  
        } else {
            if(list->count < cache->max) {
                if(node && key == node->key) {
                    // just insert into the end of node
                    struct yah_cache_node* ptr = node;
                    while(ptr->next_same != NULL) {
                        ptr = ptr->next_same;
                    }
                    ptr->next_same = newnode;
                } else {
                    // insert into the head of list
                    newnode->next = list->head;
                    list->head->prev = newnode;
                    list->head = newnode;
                }
                list->count++;
            } else {
                if(list->tail == list->head) {
                    // remove all
                    // only one node is the new node
                    struct yah_cache_node* tail = list->tail;
                    list->tail = list->head = newnode;
                    list->count = 1;
                    // remove tail
                    struct yah_cache_node* ptrs;
                    while(tail) {
                        ptrs = tail->next_same;
                        tail->destory(tail->value, tail->size);
                        free(tail);
                        tail = ptrs;
                    }
                } else {
                    // remove the tail
                    // decrease count
                    struct yah_cache_node* tail = list->tail;
                    struct yah_cache_node* prev = tail->prev;
                    prev->next = NULL;
                    list->tail = prev;

                    struct yah_cache_node* next;
                    while(tail) {
                        next = tail->next_same;
                        tail->destory(tail->value, tail->size);
                        free(tail);
                        tail = next;
                        list->count--;
                    }

                    // add new node to head
                    list->head->prev = newnode;
                    newnode->next = list->head;
                    list->head = newnode;
                    list->count++;
                }
            }
        }
    }
finish:
    pthread_mutex_unlock(&cache->mutex);
    return ret;
}
