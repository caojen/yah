#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "yah_thread_pool_manager.h"
#include "yah_log.h"

yah_thread_pool*
yah_thread_pool_init(unsigned int wnum, void* (*func)(void*)) {
    if(wnum <= 1) {
        wnum = 1;
    }
    yah_log("pool will init with num = %d", wnum);
    // init pool
    yah_thread_pool* ret =
        (yah_thread_pool*) malloc(sizeof(yah_thread_pool));
    if(ret == NULL) {
        yah_error("thread_pool_manager create thread_pool return NULL");
        return NULL;
    }
    memset(ret, 0, sizeof(yah_thread_pool));

    // init pool's job queue, remain empty
    ret->jobs = (struct yah_job_queue*) malloc (sizeof(struct yah_job_queue));
    if(yah_job_queue_init(ret->jobs) != 0) {
        // init queue failed. return NULL
        yah_error("thread_pool_manager init job queue return NULL");
        return NULL;
    }

    // init pool's workers
    ret->workers = (struct yah_worker_list*) malloc (sizeof(struct yah_worker_list));
    if(yah_worker_list_init(ret->workers) != 0) {
        // init worker list failed, return NULL
        yah_error("thread_pool_manager init workers list return NULL");
        return NULL;
    }

    // init mutex
    if(pthread_mutex_init(&ret->job_mutex, NULL) != 0) {
        yah_error("thread_pool_manager init job queue mutex error");
        return NULL;
    }

    // init cond
    if(pthread_cond_init(&ret->worker_cond, NULL) != 0) {
        yah_error("thread_pool_manager init worker cond error");
        return NULL;
    }

    // add wnum workers into workers list
    for(unsigned i = 0; i < wnum; i++) {
        struct yah_worker* worker = YAH_WORKER_INITIALIZER;
        worker->manager = ret;
        int c = pthread_create(&worker->pthread_id, NULL, func, (void*)worker);
        if(c != 0) {
            yah_error("thread_pool_manager init worker at %d failed, %s", i + 1, strerror(c));
            return NULL;
        }
        yah_worker_list_add_worker(ret->workers, worker);
    }

    return ret;
}

int
yah_thread_pool_destory(yah_thread_pool* pool) {
    int r = 0;
    r |= yah_job_queue_destory(pool->jobs);
    r |= yah_worker_list_destory(pool->workers);
    r |= pthread_mutex_destroy(&pool->job_mutex);
    r |= pthread_cond_destroy(&pool->worker_cond);

    return r;
}

int
yah_thread_pool_push_job(yah_thread_pool* pool, struct yah_job* job) {
    // try to get mutex lock to modify job queue
    pthread_mutex_lock(&pool->job_mutex);
    yah_job_queue_add_job(pool->jobs, job);
    // unlock the mutex before send cond signal
    pthread_mutex_unlock(&pool->job_mutex);
    pthread_cond_signal(&pool->worker_cond);

    return 0;
}
