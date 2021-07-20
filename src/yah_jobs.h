#ifndef YAH_JOBS_H
#define YAH_JOBS_H
#include "yah_customer.h"
#include "yah_airodump.h"

struct yah_airodump_data;
// for rp_pool
void yah_rp_pool_job_func_ap(struct yah_airodump_data* data);
void yah_rp_pool_job_func_apstation(struct yah_airodump_data* data);

// for customer:
void yah_customer_ap(struct yah_airodump_data* items[YAH_CUSTOMER_EACH_FETCH], unsigned size);
void yah_customer_apstation(struct yah_airodump_data* items[YAH_CUSTOMER_EACH_FETCH], unsigned size);
#endif
