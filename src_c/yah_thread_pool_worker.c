#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "yah_thread_pool_worker.h"

struct yah_worker*
yah_worker_init() {
    struct yah_worker* ret = 
        (struct yah_worker*) malloc (sizeof(struct yah_worker));
    if(ret != NULL) {
        memset(ret, 0, sizeof(struct yah_worker));
        // will not init other space for ret
    }

    return ret;
}

int
yah_worker_destory(struct yah_worker* worker) {
    if(worker == NULL) {
        return 1;
    }

    free(worker);
    return 0;
}

int
yah_worker_list_init(struct yah_worker_list* list) {
    if(list == NULL) {
        return 1;
    }

    memset(list, 0, sizeof(struct yah_worker_list));

    return 0;
}

int
yah_worker_list_add_worker(struct yah_worker_list* list, struct yah_worker* worker) {
    int ret = 0;
    if(list == NULL) {
        ret = 1;
    } else if(worker == NULL) {
        ret = 0;
    } else {
        ret = 0;
        list->count++;
        if(list->head == NULL) {
            list->head = list->tail = worker;
        } else {
            list->head->prev = worker;
            worker->next = list->head;
            list->head = worker;
        }
    }
    return ret;
}

unsigned int
yah_worker_list_count(struct yah_worker_list* list) {
    if(list == NULL) {
        return 0;
    }
    return list->count;
}

int
yah_worker_list_destory(struct yah_worker_list* list) {
    struct yah_worker* ptr = list->head;
    struct yah_worker* next;
    while(ptr) {
        next = ptr->next;
        yah_worker_destory(ptr);
        ptr = next;
    }
    free(list);
    return 0;
}
