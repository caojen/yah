#include <stdlib.h>
#include <unistd.h>

#include "yah_core.h"
#include "yah_log.h"
#include "yah_exec.h"
#include "yah_airodump.h"
#include "yah_pty.h"
#include "yah_const.h"
#include "yah_thread_pool.h"
#include "yah_config.h"

/**
 * make airodump_pid as global variable
 * so the signal can access and send signal if needed
 */
pid_t airodump_pid = 0;

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

    // 1. make airodump running.

    //   1.1 airodump-ng require a device name(like wlan0), firstly, get it.
    char device_name[YAH_MAX_DEVICE_NAME];
    if(get_airodump_device_name(device_name) != 0) {
        yah_quit("cannot get device name. abort.");
    } else {
        yah_log("get device name = %s", device_name);
    }

    //   1.2 init a pseudo terminal
    int airodump_fd;
    char slave_name[MAX_PTYNAME];
    struct winsize wsize;
    wsize.ws_row = 2048;
    wsize.ws_col = 2048;
    if((airodump_pid = yah_pty_fork(&airodump_fd, slave_name, MAX_PTYNAME, NULL, &wsize)) < 0) {
        yah_quit("cannot run pty_fork");
    } else if(airodump_pid == 0) {
        /* child process */
        // system call exec
        yah_log("core pty_child will call execl");
        for(int i = 0; i <100000;i++) {
            for(int j = 0; j < 100000000;j ++) {
                int t = i * j;
            }
        }
        if(execl(YAH_AIRODUMP, YAH_AIRODUMP_NAME, "-C", "2412-2472,5180-5825", "-f", "10", "--berlin", "3", NULL) == -1) {
            yah_error("core: system call exec return -1 error");
        }
        exit(127);
    }

    yah_log("core: pty_fork done, with airodump.pid = %d.", airodump_pid);
    // for(int i = 0; i <100000;i++) {
    //     for(int j = 0; j < 100000000;j ++) {
    //         int t = i * j;
    //     }
    // }

    /* parent process */
    /* continue */
    yah_log("parent process continue");

    /**
     * 4. create a pool to receive all formatted data
     * and try to post them
     * these job is adding by another pool
     */
    yah_log("before init rp_pool");
    // Receive-Post pool
    yah_thread_pool* rp_pool = yah_thread_pool_init(rpworkers, rpworker_main_func);
    yah_log("rp_pool init done");

    /**
     * 3. format each line into formatted data
     * and push to rp_pool
     * 
     * these job is adding by main thread
     */

    // Format-Push pool
    yah_thread_pool* fp_pool = yah_thread_pool_init(fpworkers, fpworker_main_func);
    yah_log("fp_pool init done");

    /**
     * 2. capture all outputs from fd
     */
    char buf[YAH_CAPTURE_LINE];
    yah_log("will call fdopen");
    FILE* fp = fdopen(airodump_fd, "r");
    // get line by line
    yah_log("getting from fd = %d, fp = %d", airodump_fd, fp);
    while(fgets(buf, YAH_CAPTURE_LINE, fp) != NULL) {
        yah_log(buf);
    }

    // use block I/O read to receive

    unimplemented();
}
