/**
 * used by yah_thread_pool.h
 * 
 * worker is a thread that try to fetch a job
 * note that is's just a list, `multi-thread` is not concerned.
 */

#ifndef YAH_THREAD_POOL_WORKER_H
#define YAH_THREAD_POOL_WORKER_H

#include <pthread.h>

#include "yah_thread_pool_manager.h"

struct yah_manager;

struct yah_worker {
    // the worker's pthread_id
    pthread_t pthread_id;
    // this worker is stop?
    // terminate = 1 means it's stopped
    int terminate;

    // next worker
    struct yah_worker* next;
    // prev worker
    struct yah_worker* prev;

    // the manager that the worker can access
    struct yah_manager* manager;
};

// init a worker
// all attr set to 0
// return the worker if success
// return NULL if failed
struct yah_worker* yah_worker_init();

// destory a worker
// return 0 if success
int yah_worker_destory(struct yah_worker* worker);

// define initializer
#define YAH_WORKER_INITIALIZER (yah_worker_init())

struct yah_worker_list {
    // the worker at the front of list
    struct yah_worker* head;
    // the worker at the end of list
    struct yah_worker* tail;
    // the number of workers in this list
    unsigned int count;
};

// init a list with empty (no job)
// list should be allocated
// return 0, if success
int yah_worker_list_init(struct yah_worker_list* list);

// add a worker into list
// return 0, if success
int yah_worker_list_add_worker(struct yah_worker_list* list, struct yah_worker* worker);

// return the number of worker in this list
unsigned int yah_worker_list_count(struct yah_worker_list* list);

// destory the list
// free this list, and free all workers in this list
// if success return 0
int yah_worker_list_destory(struct yah_worker_list* list);

#endif
