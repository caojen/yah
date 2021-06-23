#ifndef YAH_JOBS_H
#define YAH_JOBS_H

#include "yah_airodump.h"
struct yah_airodump_data;
void yah_rp_pool_job_func_ap(struct yah_airodump_data* data);
void yah_rp_pool_job_func_apstation(struct yah_airodump_data* data);

#endif
