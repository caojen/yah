/**
 * Entry of yah main function.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "yah.h"
#include "yah_const.h"
#include "yah_log.h"
#include "yah_daemon.h"
#include "yah_config.h"
#include "yah_core.h"
#include "yah_signal.h"

#ifndef TEST

int
main(int argc, char** argv) {
    yah_init_stdpipe();
    
    if(argc <= 1) {
        /* no argument supplied. print usage. */
        yah_usage();
        exit(EINVAL);
    }

    // load config file when the program start
    if(yah_reload_config() != 0) {
        yah_error("cannot load config");
        exit(1);
    }

    /* test all arguments. */
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "help") == 0 || strcmp(argv[i], "-h") == 0) {
            yah_usage();
            exit(0);
        } else if(strcmp(argv[i], "daemon") == 0) {
            yah_daemonize(1);
            yah_core_start();
            yah_log("exit from main");
            exit(0);
        } else if(strcmp(argv[i], "start") == 0) {
            /* start as daemon */
            yah_daemonize(0);
            yah_core_start();
            yah_log("exit from main");
            exit(0);
        } else if(strcmp(argv[i], "reload") == 0) {
            /* reload config */
            /* send SIGHUP to daemon */
            // get pid of the current daemon
            long daemon_pid = get_running_daemon_pid();
            if(daemon_pid == -1) {
                yah_quit("cannot get the current running daemon pid");
            } else if(daemon_pid == 0) {
                yah_quit("no daemon is running");
            } else {
                yah_log("sending SIGHUP to pid %ld", daemon_pid);
                send_signal(daemon_pid, SIGHUP);
                yah_log("done");
            }
            exit(0);
        } else if(strcmp(argv[i], "set") == 0) {
            /* set a key */
            /* set the key to config */
            /* then send SIGHUP to daemon */
            unimplemented();
            exit(0);
        } else if(strcmp(argv[i], "stop") == 0) {
            long daemon_pid = get_running_daemon_pid();
            if(daemon_pid == -1) {
                yah_quit("cannot get the current running daemon pid");
            } else if(daemon_pid == 0) {
                yah_quit("no daemon is running");
            } else {
                yah_log("sending SIGTERM to pid %ld", daemon_pid);
                send_signal(daemon_pid, SIGTERM);
                yah_log("send done, wait 2s and check kill status...");
                sleep(2);
                if(get_running_daemon_pid() != -1) {
                    yah_log("oh. it is still runnning. send SIGKILL to pid %ld", daemon_pid);
                    send_signal(daemon_pid, SIGKILL);
                    yah_log("send done, wait 2s and check kill status...");
                    if(get_running_daemon_pid() != -1) {
                        yah_error("cannot kill the current daemon. it is already running, but signal has no affect.");
                        exit(1);
                    }
                }
            }
        } else {
            yah_usage();
            exit(EINVAL);
        }
    }
    exit(0);
}

void
yah_usage(void) {
    printf("\n");
    printf("\tYah -- v1.0\n\n");
    printf("\t\tUsage: Yah [start | reload | daemon] <...>\n\n");
    printf("\t\tstart: start the program\n");
    printf("\t\tdaemon: run the program in daemon, meaning that running in the background.\n");
    printf("\t\treload: reload settings from config file: %s\n", YAH_CONFFILE);
    printf("\n");
}

void
yah_init_stdpipe(void) {
    YAH_FILENO_LOG = stdout;
    YAH_FILENO_WARN = stdout;
    YAH_FILENO_ERROR = stderr;
}

#endif
