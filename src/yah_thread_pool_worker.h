/**
 * used by yah_thread_pool.h
 * 
 * worker is a thread that try to fetch a job
 * note that is's just a list, `multi-thread` is not concerned.
 */

#ifndef YAH_THREAD_POOL_WORKER_H
#define YAH_THREAD_POOL_WORKER_H

#include <pthread.h>

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
};

struct yah_worker_list {
    // the worker at the front of list
    struct yah_worker* head;
    // the worker at the end of list
    struct yah_worker* tail;
    // the number of workers in this list
    unsigned int count;
};

#endif
