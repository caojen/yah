#include <stdlib.h>

#include "yah_core.h"
#include "yah_log.h"

/**
 * What should core do?
 * 1. make 'airodump' running. make it a coprocess 
 *    and run it in a pseudo terminal, named that terminal 'yah_airodump'
 * 2. capture all output from terminal 'yah_airodump'(the main thread do this.)
 *    main thread capture lines one by one, and convert them into jobs and push
 *    into the thread pool(see step 3.) however, main thread should put `converting`
 *    at the end of the core.(that means, main thread should finish all next steps first)
 * 3. create a thread pool to `change each line to formatted data`.
 *    that pool's job is a line, using thread to format line.
 *    besides, after formating one line, that thread convert the formated data into
 *    a new job, as the input of another thread pool(see step 4.)
 * 4. create a thread pool to `receive all formatted data` and `push that data
 *    to remote storage(using network)`. pool's job is one data, using thread to push
 * 5. create some other threads, these threads is for special functions.
 *    e.g, testing network, testing system status
 */
void yah_core_start() {
    yah_log("core is started...");

    // 1 make airodump running
}
