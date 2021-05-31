#include <pthread.h>

#include "yah_thread_pool.h"
#include "yah_log.h"

void*
yah_thread_pool_callback(void* __worker) {
    struct yah_worker* worker = (struct yah_worker*) __worker;
    yah_thread_pool* pool = worker->manager;

    while(1) {
        // try to get the pool's mutex lock
        // to modify job queue
        pthread_mutex_lock(&pool->job_mutex);
        // test if the queue is empty
        while(yah_job_queue_count(pool->jobs) == 0) {
            // the queue is empty
            // this thread sleep, wait for cond signal
            pthread_cond_wait(&pool->worker_cond, &pool->job_mutex);
        }
        // pop and get that job
        struct yah_job* job = yah_job_queue_pop_job(pool->jobs);
        // unlock mutex
        pthread_mutex_unlock(&pool->job_mutex);
        // run that job
        job->func(job->arg);
        // job done, destory that job
        yah_job_destory(job);
    }
}
