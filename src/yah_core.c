#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "yah_core.h"
#include "yah_log.h"
#include "yah_exec.h"
#include "yah_airodump.h"
#include "yah_pty.h"
#include "yah_const.h"
#include "yah_thread_pool.h"
#include "yah_config.h"
#include "yah_mem.h"
#include "yah_string.h"
#include "yah_lru.h"
#include "yah_sqlite3.h"

/**
 * make airodump_pid as global variable
 * so the signal can access and send signal if needed
 */
pid_t airodump_pid = 0;
yah_thread_pool* rp_pool = NULL;
yah_thread_pool* fp_pool = NULL;
struct yah_cache* ap_cache = NULL;
struct yah_cache* apstation_cache = NULL;
char airodump_time[YAH_CAPTURE_LINE + 1];

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
    wsize.ws_row = YAH_PTY_ROWS;
    wsize.ws_col = YAH_PTY_COLS;
    if((airodump_pid = yah_pty_fork(&airodump_fd, slave_name, MAX_PTYNAME, NULL, &wsize)) < 0) {
        yah_quit("cannot run pty_fork");
    } else if(airodump_pid == 0) {
        /* child process */
        // system call exec
        yah_log("core pty_child will call execl");
        if(execl(YAH_AIRODUMP, YAH_AIRODUMP_NAME, device_name, "-C", "2412-2472,5180-5825", "-f", "10", "--berlin", "3", NULL) == -1) {
            yah_error("core: system call exec return -1 error");
        }
        exit(127);
    }

    yah_log("core: pty_fork done, with airodump.pid = %d.", airodump_pid);

    /* parent process */
    /* continue */
    yah_log("core: parent process continue");

    /**
     * init the lru cache
     */
    yah_log("core: trying to init lru cache");
    ap_cache = yah_cache_init(64, YAH_AP_TIME, yah_string_cmp, yah_string_destory, yah_string_copy);
    apstation_cache = yah_cache_init(64, YAH_APSTATION_TIME, yah_string_cmp, yah_string_destory, yah_string_copy);
    yah_log("core: cache init done");

    /**
     * 4. create a pool to receive all formatted data
     * and try to post them
     * these job is adding by another pool
     */
    yah_log("core: trying to init thread pool...");
    // Receive-Post pool
    rp_pool = yah_thread_pool_init(rpworkers, rpworker_main_func);
    yah_log("core: rp_pool init done");

    /**
     * 3. format each line into formatted data
     * and push to rp_pool
     * 
     * these job is adding by main thread
     */

    // Format-Push pool
    fp_pool = yah_thread_pool_init(fpworkers, fpworker_main_func);
    yah_log("core: fp_pool init done");

    // open database and init it
    yah_open_database();
    yah_airodump_data_de_redundancy();
    yah_core_init_pool_data();

    /**
     * 2. capture all outputs from fd
     */
    char buf[YAH_CAPTURE_LINE];
    yah_log("core: will call fdopen");
    FILE* fp = fdopen(airodump_fd, "r");
    // get line by line
    while(fgets(buf, YAH_CAPTURE_LINE, fp) != NULL) {
        int length = strlen(buf);
        // yah_log("main: receive");
        // yah_log("main: fp_job genereating...");
        struct yah_fp_pool_job_arg* job_arg = 
            (struct yah_fp_pool_job_arg*) yah_mem_alloc (sizeof(struct yah_fp_pool_job_arg)); 
        memset(job_arg, 0, sizeof(struct yah_fp_pool_job_arg));
        job_arg->create_time = time(NULL);
        memcpy(job_arg->line, buf, length);

        // genereate fp_pool_job
        struct yah_job* job = YAH_JOB_INITIALIZER;
        // yah_log("main: fp_job_init done. setting...");
        job->arg = job_arg;
        job->arg_destory = yah_mem_free;
        job->func = yah_fp_pool_job_func;
        // yah_log("main: fp_job_setting done. pushing...");
        yah_thread_pool_push_job(fp_pool, job);
        // yah_log("main: fp_job push done. jobs remain = %d", yah_job_queue_count(fp_pool->jobs));
        // yah_log("main: fp_job push to fp_pool done...");
    }

    unreachable();
}

void
yah_fp_pool_job_func(void* __arg) {
    struct yah_fp_pool_job_arg* arg =
        (struct yah_fp_pool_job_arg*) __arg;

    char* line = arg->line;
    char* ptr = line;
    // get the first \n, trunc it
    while(*ptr) {
        if(*ptr == '\n') {
            *ptr = 0;
            break;
        }
        ++ptr;
    }
    ptr = line;
    // remove all invisiable charactor in arg->line
    // remove heading or tailing blank
    // remain multi blanks
    // move the line into newline
    char newline[YAH_CAPTURE_LINE + 1] = { 0 };
    char* nptr = newline;
    ptr = line;
    int heading = 1;
    while(*ptr) {
        if(*ptr == ESC) {
            while(*ptr && *ptr != ' ') {
                ++ptr;
            }
        }
        if(*ptr == 0) {
            break;
        }
        while(*ptr && heading && *ptr == ' ') {
            ++ptr;
        }
        heading = 0;
        *nptr = *ptr;
        ++nptr;
        ++ptr;
    }
    int length = strlen(newline);
    nptr = newline + length - 1;
    while(nptr > newline && *nptr == ' ') {
        --nptr;
        --length;
    }
    *(nptr + 1) = 0;
    // if the line is too short, ignore it.
    // yah_log("fp_job_func: receive arg with line.length = %d", length);
    if(length < YAH_CAPTURE_MIN_LINE) {
        // ignore this line
        return;
    }

    // continue
    // if newline begin with "BSSID" / "FREQ", ignore
    if(yah_string_prefix(newline, "BSSID") ||
        yah_string_prefix(newline, "(not associated)") || yah_string_prefix(newline, "available") || 
        yah_string_substring(newline, "<length:")) {
        // ignore it
        return;
    }
    if(yah_string_prefix(newline, "Freq")) {
        // here we get the time that airodump output this line
        // note that the create_time is the time that we catch the output
        // but not the time that airodump show the output
        // pass -D DEBUG to turn it on
#ifdef DEBUG
        strcpy(airodump_time, newline);
#endif
        return;
    }
    // yah_log("%s", newline);
    char* first_part_begin;
    char* first_part_end;

    char* second_part_begin;
    char* second_part_end;

    yah_string_get_next_part(newline, &first_part_begin, &first_part_end);
    yah_string_get_next_part(first_part_end, &second_part_begin, &second_part_end);
    
    *first_part_end = 0;    // first part is the bssid
    *second_part_end = 0;
    int first_part_length = first_part_end - first_part_begin;
    int second_part_length = second_part_end - second_part_begin;

    struct yah_airodump_data* data = NULL;
    if(second_part_length > 10) {
        enum yah_airodump_type type = apstation;
        int incache = yah_cache_update(apstation_cache, first_part_begin, first_part_length + 1);
        if(incache == YAH_CACHE_NODE_NOTEXISTS) {
            yah_log("push type = %d, bssid = %s", type, first_part_begin);
            // yah_log("current airodump time = %s", airodump_time);
            data = (struct yah_airodump_data*) yah_mem_alloc (sizeof(struct yah_airodump_data));
            memset(data, 0, sizeof(struct yah_airodump_data));
            data->type = apstation;
            strcpy(data->data.apstation.bssid, first_part_begin);
            strcpy(data->specify, second_part_begin);
            strcpy(data->data.apstation.station, second_part_begin);
            *first_part_end = ' ';
            *second_part_end = ' ';
            strcpy(data->data.apstation.comment, newline);
            data->data.apstation.create_time = arg->create_time;
        }
    } else {
        enum yah_airodump_type type = ap;
        int incache = yah_cache_update(ap_cache, first_part_begin, first_part_length + 1);
        if(incache == YAH_CACHE_NODE_NOTEXISTS) {
            yah_log("push type = %d, bssid = %s", type, first_part_begin);
            // yah_log("current airodump time = %s", airodump_time);
            data = (struct yah_airodump_data*) yah_mem_alloc (sizeof(struct yah_airodump_data));
            memset(data, 0, sizeof(struct yah_airodump_data));
            data->type = ap;
            strcpy(data->data.ap.bssid, first_part_begin);
            strcpy(data->specify, first_part_begin);
            *first_part_end = ' ';
            *second_part_end = ' ';
            strcpy(data->data.ap.comment, newline);
            data->data.ap.create_time = arg->create_time;
        }
    }
    if(data != NULL) {
        // generate that job, and push to rp_pool's job queue
        struct yah_job* job = YAH_JOB_INITIALIZER;
        job->arg = (void*) data;
        job->arg_destory = yah_mem_free;
        job->func = yah_rp_pool_job_func;
        yah_thread_pool_push_job(rp_pool, job);
    }
}

void
yah_rp_pool_job_func(void* __arg) {
    struct yah_airodump_data* arg = 
        (struct yah_airodump_data*) __arg;
    yah_log("rp_pool_job: resolving type = %d, specify = %s", arg->type, arg->specify);
    // update database and get id
    int should_insert = (
        yah_airodump_data_insert(arg)
    ) == 0;
    if(should_insert) {
        switch (arg->type) {
            case ap:
                yah_log("rp_pool_job: genereate id for type = %d, specify = %s, id = %d", arg->type, arg->specify, arg->data.ap.id);
                yah_rp_pool_job_func_ap(arg);
                break;
            case apstation:
                yah_log("rp_pool_job: genereate id for type = %d, specify = %s, id = %d", arg->type, arg->specify, arg->data.apstation.id);
                yah_rp_pool_job_func_apstation(arg);
                break;
            default:
                unreachable();
        }
    } else {
        // skip this job
        // yah_log("rp_pool_job: skip type = %d, specify = %s", arg->type, arg->specify);
    }
}

void
yah_rp_pool_job_func_ap(struct yah_airodump_data* data) {
    // TODO: upload the data to remote

    // set data is_uploaded in database
    yah_airodump_data_updated(data);
}

void
yah_rp_pool_job_func_apstation(struct yah_airodump_data* data) {
    // TODO: upload the data to remote

    // set data is uploaded in database
    yah_airodump_data_updated(data);
}

void yah_core_init_pool_data() {
    struct yah_airodump_data* data;
    unsigned size = 0;
    if(yah_airodump_data_fetch_unupdated(&data, &size) != 0) {
        yah_quit("cannot init from pool. exit...");
    }

    yah_log("get old data size = %d", size);
    for(unsigned i = 0; i < size; i++) {
        // generate that job, and push to rp_pool's job queue
        struct yah_job* job = YAH_JOB_INITIALIZER;
        yah_log("%d: old job: type = %d, specify = %s", i + 1, data[i].type, data[i].specify);
        job->arg = (void*) (data + i);
        // job->arg_destory = yah_mem_free;
        job->arg_destory = NULL;    // memory leap here. TODO: find some ways to debug
        job->func = yah_rp_pool_job_func;
        yah_thread_pool_push_job(rp_pool, job);
    }
}
