/**
 * all functions that used in daemon mode
 * 
 * entry: yah_core_start()
 */

#ifndef YAH_CORE_H
#define YAH_CORE_H

// start in daemon mode
// do everything that the process should do
void yah_core_start();

// the main function for worker
// void* rpworker_main_func(void* worker);
#define rpworker_main_func yah_thread_pool_callback
#define fpworker_main_func yah_thread_pool_callback

#define YAH_CAPTURE_LINE 4096

#endif
