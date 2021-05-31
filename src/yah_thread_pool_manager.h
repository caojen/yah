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

#endif
