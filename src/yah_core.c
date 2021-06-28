#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <zlib.h>
#include <errno.h>
#include <sys/wait.h>

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
#include "yah_json.h"
#include "yah_base64.h"
#include "yah_http.h"
#include "cjson.h"

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
char remote_ip[20] = { 0 };
int remote_port = 0;

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

    //   1.2 init a pseudo terminal, for running airodump-ng
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


    // init remote_ip and remote_port
    // store in global variable remote_ip and remote_port
    // (system call gethostbyname() used)
    yah_init_remote();

    /**
     * init two lru caches. for ap and apstation.
     * 
     * these two caches storing into global variable ap_cache and apstation_cache
     */
    ap_cache = yah_cache_init(64, YAH_AP_TIME, yah_string_cmp, yah_string_destory, yah_string_copy);
    apstation_cache = yah_cache_init(64, YAH_APSTATION_TIME, yah_string_cmp, yah_string_destory, yah_string_copy);

    /**
     * 4. create a pool to receive all formatted data
     * and try to post them
     * these job is adding by another pool
     */
    // Receive-Post pool
    rp_pool = yah_thread_pool_init(rpworkers, rpworker_main_func);

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
    // the database path is defined in yah_config.h.
    yah_open_database();
    // remove old data to minimize the database
    yah_airodump_data_de_redundancy();
    // call init sql (create table if not exists....) to init the database
    yah_core_init_pool_data();

    /**
     * 2. capture all outputs from fd
     * fd is the file descriptor from pty(where airodump-ng is running)
     */
    const int bufsize = YAH_CAPTURE_LINE + 3;
    char buf[bufsize];
    int length = 0;
    // yah_log("core: will call fdopen");
    // use select to check airodump_fd is ok
    fd_set input_set;
    FD_ZERO(&input_set);
    FD_SET(airodump_fd, &input_set);
    struct timeval timeout; timeout.tv_sec = 15; timeout.tv_usec = 0; // create timeout to 15s
    int ready_for_reading;
    while(1) {
        ready_for_reading = select(1, &input_set, NULL, NULL, &timeout);
        if(ready_for_reading == -1) {
            // read error. check the airodump-ng
            yah_log("core: system call select timeout of %ds", timeout.tv_sec);
            yah_log("core: to check airodump-ng with pid = %d is running", airodump_pid);
            int status;
            pid_t result = waitpid(airodump_pid, &status, WNOHANG);
            if(result == 0) {
                // airodump-ng is still running. do nothing.
                yah_log("core: airodump-ng is still running.");
            } else if(result == -1) {
                // Error
                yah_log("core: checking airodump-ng subprocess error, with result = -1");
            } else {
                yah_log("core: airodump-ng has exited. the parent process should exit.");
                daemon_exit();
            }
        } else {
            // read ok. just proceed.
            // read one line into buf
            char ch;
            char* bufptr = buf;
            int r;
            while(1) {
                while((r = read(airodump_fd, &ch, 1)) == -1 && errno == EINTR);
                if(ch == '\n') {
                    *bufptr = ch;
                    *(bufptr + 1) = 0;
                    length = bufptr - buf;
                    break;
                } else {
                    *bufptr = ch;
                    ++bufptr;
                }
            }
            // store this line to buf, with strlen(buf) == length
            // if length < 10, ignore this line
            if(length >= 10) {
                struct yah_fp_pool_job_arg* job_arg = 
                    (struct yah_fp_pool_job_arg*) yah_mem_alloc (sizeof(struct yah_fp_pool_job_arg)); 
                memset(job_arg, 0, sizeof(struct yah_fp_pool_job_arg));
                job_arg->create_time = time(NULL);
                memcpy(job_arg->line, buf, length);

                // genereate fp_pool_job
                struct yah_job* job = YAH_JOB_INITIALIZER;
                job->arg = job_arg;
                job->arg_destory = yah_mem_free;
                job->func = yah_fp_pool_job_func;
                yah_thread_pool_push_job(fp_pool, job);
            }
        }
    }
    yah_warn("core: out-of-loop, reach the unreachable code!");
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
    // yah_log("fp_job_func: receive line: %s", newline);
    if(length < YAH_CAPTURE_MIN_LINE) {
        // ignore this line
        return;
    }

    // if newline begin with "BSSID" / "FREQ" / ..., ignore
    if(
        // not a valid line:
        yah_string_prefix(newline, "BSSID") || yah_string_prefix(newline, "available") || 
        // not usable data
        // yah_string_prefix(newline, "(not associated)") || 
        // network broken:
        yah_string_prefix(newline, "failed:") || yah_string_prefix(newline, "ioctl(SIOCGIFINDEX)")
        // not usable data:
        // yah_string_substring(newline, "<length:")
    ) {
        
        return;

    }
    // From "Freq", we can get the time.
    if(yah_string_prefix(newline, "Freq")) {
        // here we get the time that airodump output this line
        // note that the create_time is the time that we catch the output
        // but not the time that airodump show the output
        // pass -D DEBUG to turn it on
// #ifdef DEBUG··
        strcpy(airodump_time, newline);
// #endif
        return;
    }
    // yah_log("%s", newline);
    char* first_part_begin;
    char* first_part_end;

    char* second_part_begin;
    char* second_part_end;

    yah_string_get_next_part(newline, &first_part_begin, &first_part_end);
    yah_string_get_next_part(first_part_end, &second_part_begin, &second_part_end);

    if(yah_string_prefix(newline, "(not associated)")) {
        first_part_end = second_part_end;
        yah_string_get_next_part(first_part_end, &second_part_begin, &second_part_end);
    }
    
    *first_part_end = 0;    // first part is the bssid
    *second_part_end = 0;
    int first_part_length = first_part_end - first_part_begin;
    int second_part_length = second_part_end - second_part_begin;

    struct yah_airodump_data* data = NULL;
    if(second_part_length > 10) {
        enum yah_airodump_type type = apstation;
        int incache = yah_cache_update(apstation_cache, second_part_begin, second_part_length + 1);
        if(incache == YAH_CACHE_NODE_NOTEXISTS) {
            yah_log("fp_pool_job: will push to rp_pool type = %d, bssid = %s", type, first_part_begin);
            yah_log("fp_pool_job: apstation, %s, %s", first_part_begin, second_part_begin);
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
            yah_log("fp_pool_job: will push to rp_pool type = %d, bssid = %s", type, first_part_begin);
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
    // data is generated. check if data->specify(usually the bssid or the apstation)'s length is 17 or not.
    if(data != NULL && strlen(data->specify) == 17) {
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
    yah_log("rp_pool_job: %p, resolving type = %d, specify = %s, device = %d", arg, arg->type, arg->specify, device_number);
    // update database and get id
    int should_insert = (
        yah_airodump_data_insert(arg)
    ) == 0;
    if(should_insert) {
        switch (arg->type) {
            case ap:
                yah_log("rp_pool_job: %p, genereate id for type = %d, specify = %s, id = %d", arg, arg->type, arg->specify, arg->data.ap.id);
                yah_rp_pool_job_func_ap(arg);
                break;
            case apstation:
                yah_log("rp_pool_job: %p, genereate id for type = %d, specify = %s, id = %d", arg, arg->type, arg->specify, arg->data.apstation.id);
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
yah_core_init_pool_data() {
    struct yah_airodump_data** data;
    unsigned size = 0;
    const int maxsize = 100;

    // get all is_uploaded = 0
    // make them as jobs. push to rp_pool
    if(yah_airodump_data_fetch_unupdated(&data, &size) != 0) {
        yah_quit("cannot init from pool. exit...");
    }

    yah_log("get old data size = %d", size);

    struct yah_airodump_data** aps = (struct yah_airodump_data**) malloc (sizeof(struct yah_airodump_data*) * (maxsize + 1));
    memset(aps, 0, sizeof(struct yah_airodump_data*) * maxsize + 1);
    struct yah_airodump_data** apstations = (struct yah_airodump_data**) malloc (sizeof(struct yah_airodump_data*) * (maxsize + 1));
    memset(apstations, 0, sizeof(struct yah_airodump_data*) * (maxsize + 1));

    int ctr_aps = 0;
    int ctr_apstations = 0;

    for(unsigned i = 0; i < size; i++) {
        switch (data[i]->type) {
            case ap: {
                aps[ctr_aps++ % maxsize] = data[i];
                if(ctr_aps % maxsize == 0) {
                    struct yah_job* job = YAH_JOB_INITIALIZER;
                    job->arg = (void*) aps;
                    job->arg_destory = free;
                    job->func = yah_rp_pool_job_func_aps;
                    yah_thread_pool_push_job(rp_pool, job);
                    aps = (struct yah_airodump_data**) malloc (sizeof(struct yah_airodump_data*) * (maxsize + 1));
                    memset(aps, 0, sizeof(struct yah_airodump_data*) * (maxsize + 1));
                }
                break;
            }
            case apstation: {
                apstations[ctr_apstations++ % maxsize] = data[i];
                if(ctr_apstations % maxsize == 0) {
                    struct yah_job* job = YAH_JOB_INITIALIZER;
                    job->arg = (void*) apstations;
                    job->arg_destory = free;
                    job->func = yah_rp_pool_job_func_apstations;
                    yah_thread_pool_push_job(rp_pool, job);
                    apstations = (struct yah_airodump_data**) malloc (sizeof(struct yah_airodump_data*) * (maxsize + 1));
                    memset(apstations, 0, sizeof(struct yah_airodump_data*) * (maxsize + 1));
                }
                break;
            }
        }
    }
    if(ctr_aps % maxsize) {
        struct yah_job* job = YAH_JOB_INITIALIZER;
        job->arg = (void*) aps;
        job->arg_destory = free;
        job->func = yah_rp_pool_job_func_aps;
        yah_thread_pool_push_job(rp_pool, job);
    }
    if(ctr_apstations % maxsize) {
        struct yah_job* job = YAH_JOB_INITIALIZER;
        job->arg = (void*) apstations;
        job->arg_destory = free;
        job->func = yah_rp_pool_job_func_apstations;
        yah_thread_pool_push_job(rp_pool, job);
    }
    yah_log("old_job fetch ended..");
}

void
yah_init_remote() {
    const char* address = YAH_REMOTE_HOST;
    struct hostent* host = gethostbyname(address);
    yah_log("remote: get host by name: %s", address);
    if(host == NULL) {
        yah_log("remote warning: host is null. %s", hstrerror(h_errno));
    } else {
        yah_log("remote: alias: %s", host->h_name);
        
        char* ip = inet_ntoa(*(struct in_addr*)host->h_addr);
        yah_log("remote: ip: %s", ip);
        memcpy(remote_ip, ip, strlen(ip));
    }
    remote_port = YAH_REMOTE_PORT;
}

void
yah_get_remote_local(char __remote_ip[20]) {
    const char* address = YAH_REMOTE_HOST;
    struct hostent* host = gethostbyname(address);
    if(host) {
        char* ip = inet_ntoa(*(struct in_addr*)host->h_addr);
        memcpy(__remote_ip, ip, strlen(ip));
        yah_log("dns: %s => %s", address, __remote_ip);
    } else {
        yah_log("dns warning. get host by name failed: %s", hstrerror(h_errno));
        __remote_ip[0] = 0;
    }
}
