#ifndef YAH_CUSTOMER_H
#define YAH_CUSTOMER_H

#include "yah_airodump.h"

#define YAH_CUSTOMER_QUEUE_SIZE 10000
#define YAH_CUSTOMER_EACH_FETCH 100

struct __yah_customer {
  struct yah_airodump_data * items[YAH_CUSTOMER_QUEUE_SIZE];
  unsigned begin, end;
  pthread_mutex_t mutex;
  void (*func)();
};

typedef struct __yah_customer yah_customer;

#define customer_is_full(c) (c->begin == (c->end + 1) % YAH_CUSTOMER_QUEUE_SIZE)
#define customer_is_empty(c) (c->begin == c->end)
#define customer_next(n) (((n) + 1) % YAH_CUSTOMER_QUEUE_SIZE)
#define customer_current(c) (c->items[c->begin])

// init a customer
yah_customer* yah_customer_init(void (*func)());
// push an item in customer
// block if the cache is full
void yah_customer_push(yah_customer*, struct yah_airodump_data*);
// fetch items from customer
// block if the cache is used
// return null if no data
// store the data in *ret, with size = *size
void yah_customer_fetch(yah_customer*, struct yah_airodump_data*[YAH_CUSTOMER_EACH_FETCH], unsigned *size);

#endif
