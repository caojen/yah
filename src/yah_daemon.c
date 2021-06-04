#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/resource.h>

#include "yah_const.h"
#include "yah_log.h"
#include "yah_daemon.h"
#include "yah_error.h"
#include "yah_config.h"
#include "yah_signal.h"
#include "yah_core.h"

int
yah_daemonize(int daemon) {
    umask(0);

    int isroot = check_is_root();
    if(isroot == ~YAH_RUNNING_AS_ROOT) {
        if(YAH_ROOT_REQUIRED) {
            YAH_ERROR(YAH_E_SHOULD_BE_ROOT);
            exit(3);
        } else {
            yah_warn("you are not root. some steps may fail.");
        }
    }
#if YAH_CHECK_AIRODUMP
    int has_airodump = check_airodump_exists();
    if(has_airodump != YAH_AIRODUMP_EXISTS) {
        YAH_ERROR(YAH_E_AIRODUMP_NOT_FOUND);
        exit(1);
    }
#endif

    /**
     * Get maximunm number of file descriptors
     */
    struct rlimit rl;
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        yah_quit("cannot get the maximun number of file descriptors");
    }

    if(daemon == 1) {
        /**
         * fork and use setsid()
         * In this way, the new process will become a session leader,
         *  and no tty will connect to it.
         */
        pid_t pid;
        pid = fork();
        if(pid < 0) {
            yah_quit("cannot fork a new subprocess");
        } else if(pid > 0) { /* parent process */
            yah_log("parent process exit...");
            exit(0);
        }

        /* child process */
        setsid();
        /* ignore SIG_IGN */
        struct sigaction sa;
        sa.sa_handler = SIG_IGN;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if(sigaction(SIGHUP, &sa, NULL) < 0) {
            yah_quit("cannot ignore SIG_IGN");
        }
        if((pid = fork()) < 0) {
            yah_quit("cannot fork a new subprocess");
        } else if(pid > 0) { /* parent process */
            yah_log("parent process exit...");
            yah_log("see:");
            yah_log("\t%s", YAH_LOGFILE_LOG);
            yah_log("\t%s", YAH_LOGFILE_WARN);
            yah_log("\t%s", YAH_LOGFILE_ERROR);
            yah_log(" for more starting status");
            exit(0);
        }

        /* final child process */
    }

    /* close all file descriptors */
    if(rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    for(int i = 0; i < rl.rlim_max; i++) {
        close(i);
    }
    /* attach file descriptors 0, 1, 2 */
    /* 0 -> /dev/null */
    /* 1 -> /dev/null */
    /* 2 -> /dev/null */
    int fd0 = open("/dev/null", O_RDWR);
    int fd1 = dup(fd0);
    int fd2 = dup(fd0);
    /**
     * to test if fd0~fd2 is 0~2
     * the stdout, stderr is closed, so just exit(EPIPE)
     */
    if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
        exit(EPIPE);
    }

    // move old file to other names
    rename_logs();
    
    /* init FILE* for logs */
    YAH_FILENO_LOG = fopen(YAH_LOGFILE_LOG, "w");
    YAH_FILENO_WARN = fopen(YAH_LOGFILE_WARN, "w");
    YAH_FILENO_ERROR = fopen(YAH_LOGFILE_ERROR, "w");

    // if one of them is NULL, exit with EPIPE error
    if(!YAH_FILENO_LOG || !YAH_FILENO_WARN || !YAH_FILENO_ERROR) {
        exit(EPIPE);
    }

    int isrunning = check_daemon_running();
    if(isrunning == YAH_DAEMON_RUNNING) {
        YAH_ERROR(YAH_E_ALREADY_RUNNINT);
        exit(1);
    }

    /* change the dir to '/' */
    if(chdir("/") < 0) {
        yah_quit("cannot chdir to /");
    }

    int handlers;
    if((handlers = signal_handler_register()) < 0) {
        yah_quit("Some signal handler registered failed");
    }
    yah_log("register signal success: %d", handlers);

    yah_log("done. daemon started");
    return 0;
}

int
check_daemon_running(void) {
    int fd = open(YAH_LOCKFILE, O_RDWR | O_CREAT, YAH_LOCKMODE);
    if(fd < 0) {
        yah_error("cannot open %s: %s", YAH_LOCKFILE, strerror(errno));
        exit(1);
    }

    if(lockfile(fd) < 0) {
        if(errno == EAGAIN || errno == EACCES) {
            close(fd);
            return YAH_DAEMON_RUNNING;
        } else {
            yah_error("cannot lock %s: %s", YAH_LOCKFILE, strerror(errno));
            exit(1);
        }
    }

    // write self.pid to fd
    long pid = getpid();
    ftruncate(fd, 0);
    char buf[16];
    sprintf(buf, "%ld", pid);
    write(fd, buf, strlen(buf) + 1);
    yah_log("lockfile: ok. pid = %ld", pid);
    return YAH_DAEMON_NOTRUNNING;
}

int
check_airodump_exists(void) {
    // test if we can access YAH_AIRODUMP with X_OK
    int can_access = access(YAH_AIRODUMP, X_OK);
    int ret = -1;
    if(can_access == 0) {
        ret = YAH_AIRODUMP_EXISTS;
    } else {
        ret = ~YAH_AIRODUMP_EXISTS;
    }
    return ret;
}

int
lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence  = SEEK_SET;
    fl.l_len = 0;
    int ret = fcntl(fd, F_SETLK, &fl);

    return ret;
}

int
check_is_root(void) {
    uid_t id = getuid();
    if(id == 0) {
        return YAH_RUNNING_AS_ROOT;
    } else {
        return ~YAH_RUNNING_AS_ROOT;
    }
}

long
get_running_daemon_pid(void) {
    // test if lockfile exists:
    if(access(YAH_LOCKFILE, R_OK) != 0) {
        // no such file
        YAH_ERROR(YAH_E_NO_SUCH_FILE);
        return -1;
    }
    if(check_daemon_running() == YAH_DAEMON_NOTRUNNING) {
        return 0;
    }
    // open this file:
    int fd = open(YAH_LOCKFILE, O_RDONLY);
    if(fd < 0) {
        yah_error("cannot open %s: %s", YAH_LOCKFILE, strerror(errno));
    }

    // daemon is running
    // read fd to get the pid(a string)
    char pid[YAH_MAXLINE];
    if(read(fd, pid, YAH_MAXLINE) < 0) {
        yah_error("cannot read from %s: %s", YAH_LOCKFILE, strerror(errno));
    }
    yah_log("get current running daemon pid = %s", pid);
    return atoi(pid);
}

void
daemon_exit(void) {
    // TODO: do everything before exiting with SIGTERM
    if(airodump_pid != 0) {
        yah_log("sending SIGKILL to airodump.pid = %d", airodump_pid);
        send_signal(airodump_pid, SIGKILL);
    }
    exit(SIGTERM);
}
