#include <stdlib.h>
#include <string.h>

#include "yah_thread_pool_job.h"

int
yah_job_queue_init(struct yah_job_queue* queue) {
    memset(queue, 0, sizeof(struct yah_job_queue));

    return 0;
}

int
yah_job_queue_add_job(struct yah_job_queue* queue, struct yah_job* job) {
    if(job != NULL) {
        if(queue->head == NULL) {
            queue->head = job;
            queue->tail = job;
        } else {
            job->next = queue->head;
            queue->head->prev = job;
            queue->head = job;
        }
        queue->count++;
    }

    return 0;
}

struct yah_job*
yah_job_queue_pop_job(struct yah_job_queue* queue) {
    struct yah_job*  ret = NULL;
    if(queue->head == NULL) {
        ret = NULL;
    } else if(queue->head == queue->tail) {
        ret = queue->head;
        ret->prev = ret->next = NULL;
        queue->head = queue->tail = NULL;
        queue->count--;
    } else {
        ret = queue->tail;
        ret->prev = ret->next = NULL;
        queue->tail->prev->next = NULL;
        queue->count--;
    }

    return ret;
}

unsigned int
yah_job_queue_count(struct yah_job_queue* queue) {
    return queue->count;
}

int
yah_job_queue_destory(struct yah_job_queue* queue) {
    struct yah_job* job = queue->head;
    struct yah_job* next;
    while(job != NULL) {
        next = job->next;
        if(job->arg_destory != NULL && job->arg != NULL) {
            job->arg_destory(job->arg);
        }
        if(job->destory != NULL) {
            job->destory(job);
        }
        job = next;
    }
    return yah_job_queue_init(queue);
}
