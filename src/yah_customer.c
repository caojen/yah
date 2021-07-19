#include "yah_customer.h"

yah_customer* yah_customer_init(void (*func)()) {
  yah_customer* ret = (yah_customer*) malloc (sizeof(yah_customer));
  memset(ret, 0, sizeof(yah_customer));
  pthread_mutex_init(&ret->mutex, NULL);
  ret->func = func;

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
