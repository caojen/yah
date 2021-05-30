/**
 * Yah use thread pool to
 *  1. fetch stdout from coprocess `airodump`, which is `job`(many thread convert the stdout to job)
 *  2. the job will be stored into database by one thread
 *  3. after stored, the job will be thrown into thread_pool's job queue by that thread
 *  4. multi-thread will fetch all jobs, and use network to send data in that job.
 * 
 * Yah treats that: 
 *  1. storing data into database(i.e, our filesystem) is quick.
 *  2. network is unstable. It is slow and errors can be caught at any time.
 * 
 * Thread pool's config is stored in yat_config.c/.h, which can be loaded from config file.
 * 
 * When Yah starts, it tries to fetch all data in database that mark `un-uploaded`,
 * and construct them into jobs which will be thrown into thread pool and be collected
 * by some thread.
 * 
 * When Yah stops, because all jobs's data are stored in database, data will not lose.
 */

#ifndef YAH_THREAD_POOL_H
#define YAH_THREAD_POOL_H

#include "yah_thread_pool_job.h"

#endif
