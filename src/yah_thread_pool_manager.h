/**
 * used by yah_thread_pool.h
 * 
 * manage the queue and worker
 */

#ifndef YAH_THREAD_POOL_MANAGER_H
#define YAH_THREAD_POOL_MANAGER_H

#include <pthread.h>

#include "yah_thread_pool_job.h"
#include "yah_thread_pool_worker.h"

struct yah_manager {
    // the job queue that will be managed
    struct yah_job_queue* jobs;
    // the worker list that will be managed
    struct yah_worker_list* workers;

    // mutex that to manage the job queue
    pthread_mutex_t job_mutex;
    // cond that to manage the workers
    pthread_cond_t worker_cond;
};

typedef struct yah_manager yah_thread_pool;

// init a thread pool
// wnum is the number of workers
// func is the worker's function, it receive the worker itself as the only argument
// a finished func is defined in yah_thread_pool.h
// return the thread pool. return NULL if failed.
// after this function called, the workers are started.
// just use `yah_thread_pool_push_job` and the worker will
// fetch jobs and run jobs->func(jobs->args)
yah_thread_pool* yah_thread_pool_init(unsigned int wnum, void* (*func)(void*));

// destory the thread pool
// return 0 if success
int yah_thread_pool_destory(yah_thread_pool* pool);

// try to push a job into thread pool
// return 0 if success
int yah_thread_pool_push_job(yah_thread_pool* pool, struct yah_job* job);

#endif
