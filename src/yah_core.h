/**
 * all functions that used in daemon mode
 * 
 * entry: yah_core_start()
 */

#ifndef YAH_CORE_H
#define YAH_CORE_H

#include "yah_pty.h"

// start in daemon mode
// do everything that the process should do
void yah_core_start();

// the main function for worker
// void* rpworker_main_func(void* worker);
#define rpworker_main_func yah_thread_pool_callback
#define fpworker_main_func yah_thread_pool_callback

#define YAH_CAPTURE_LINE YAH_PTY_COLS

// the min-limit of each capture line
// if strlen(line) < min-limit, ignore this line
#define YAH_CAPTURE_MIN_LINE 20

extern pid_t airodump_pid;

#include "yah_thread_pool.h"
extern yah_thread_pool* rp_pool;
extern yah_thread_pool* fp_pool;

// the main func that formatted-push pool's job do
// __arg: a `struct yah_fp_pool_job_arg*`
// formatted arg, then, push to rp_pool
void yah_fp_pool_job_func(void* __arg);

// the main func that receive-post pool's job do
// __arg: a `struct yah_airodump_data*`
// confirm that __arg did not store in database
// after confirmed, push to network
void yah_rp_pool_job_func(void* __arg);

// init fp_pool and rp_pool from database
void yah_core_init_pool_data();

#include "yah_airodump.h"
struct yah_airodump_data;
void yah_rp_pool_job_func_ap(struct yah_airodump_data* data);
void yah_rp_pool_job_func_apstation(struct yah_airodump_data* data);

// init remote_ip and remote_port
// from yah_const.h: YAH_REMOTE_HOST YAH_REMOTE_PORT
void yah_init_remote();
void yah_get_remote_local(char __remote_ip[20]);

#endif
