#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "yah_customer.h"
#include "yah_log.h"

#define CUSTOMER_THREAD_INIT_PERIOD 2

static void* yah_customer_main_thread(void * __c) {
  yah_customer* c = (yah_customer*) __c;
  unsigned timewait = CUSTOMER_THREAD_INIT_PERIOD;
  while(1) {
    struct yah_airodump_data* items[YAH_CUSTOMER_EACH_FETCH];
    unsigned size = 0;
    yah_customer_fetch(c, items, &size);
    if(size == 0) {
      timewait += 10;
    } else {
      if(size < YAH_CUSTOMER_EACH_FETCH) {
        timewait = CUSTOMER_THREAD_INIT_PERIOD;
      } else {
        timewait = 0;
      }
      c->func(items, size);
    }

    if(timewait > 0) {
      sleep(timewait);
    }
  }

  return NULL;
}

yah_customer* yah_customer_init(void (*func)(struct yah_airodump_data*[YAH_CUSTOMER_EACH_FETCH], unsigned)) {
  yah_customer* ret = (yah_customer*) malloc (sizeof(yah_customer));
  memset(ret, 0, sizeof(yah_customer));
  pthread_mutex_init(&ret->mutex, NULL);
  ret->func = func;

  // init one thread
  int c = pthread_create(&ret->pthread_id, NULL, yah_customer_main_thread, ret);
  yah_log("customer init thread %lu, ret = %d", ret->pthread_id, c);
  return ret;
}

void yah_customer_push(yah_customer* c, struct yah_airodump_data* item) {
  pthread_mutex_lock(&c->mutex);

  if(!customer_is_full(c)) {
    c->items[c->end] = item;
    c->end = customer_next(c->end);
  }

  pthread_mutex_unlock(&c->mutex);
}

void yah_customer_fetch(yah_customer* c, struct yah_airodump_data* ret[YAH_CUSTOMER_EACH_FETCH], unsigned *size) {
  pthread_mutex_lock(&c->mutex);

  *size = 0;
  while(!customer_is_empty(c)) {
    ret[*size] = customer_current(c);
    *size += 1;
    c->begin = customer_next(c->begin);
  }

  pthread_mutex_unlock(&c->mutex);
}
