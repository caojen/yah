/**
 * used by yah_thread_pool.h
 * note that it's just a queue, `multi-thread` is not concerned.
 */

#ifndef YAH_THREAD_POOL_JOB_H
#define YAH_THREAD_POOL_JOB_H

struct yah_job {
    // the function that this job should do
    void (*func)(void* arg);
    // the argument that will be passed to func
    void* arg;
    // the way to destory arg:
    void (*arg_destory)(void* arg);

    // next job
    struct yah_job* next;
    // prev job
    struct yah_job* prev;
};

// init a job
// return that job if success
// return NULL if failed
inline struct yah_job* yah_job_init(void);

// destory a job
// will destory job->arg, if arg isn't null and arg_destory isn't null
// return 0 if success
inline int yah_job_destory(struct yah_job* job);

struct yah_job_queue {
    // the job at the front of queue
    struct yah_job* head;
    // the job at the end of queue
    struct yah_job* tail;
    // the number of jobs in this queue
    unsigned int count;
};

// init a queue with empty (no job).
// queue should be allocated.
// if success, return 0
inline int yah_job_queue_init(struct yah_job_queue* queue);

// add a new job into queue
// if job is NULL, nothing happened
// if success return 0
int yah_job_queue_add_job(struct yah_job_queue* queue, struct yah_job* job);

// pop a job from queue
// if no job can be supplied, return NULL
struct yah_job* yah_job_queue_pop_job(struct yah_job_queue* queue);

// count how many job are here in this queue
inline unsigned int yah_job_queue_count(struct yah_job_queue* queue);

// destory this queue.
// will not free this queue, just free all jobs still in this queue.
// if success, return 0
int yah_job_queue_destory(struct yah_job_queue* queue);
#endif
