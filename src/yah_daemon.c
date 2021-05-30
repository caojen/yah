#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

#include "yah_const.h"
#include "yah_log.h"
#include "yah_daemon.h"
#include "yah_error.h"

int
yah_daemonize(void) {
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

    int isrunning = check_daemon_running();
    if(isrunning == YAH_DAEMON_RUNNING) {
        YAH_ERROR(YAH_E_ALREADY_RUNNINT);
        exit(1);
    }

    // int has_airodump = check_airodump_exists();
    // if(has_airodump != YAH_AIRODUMP_EXISTS) {
    //     YAH_ERROR(YAH_E_AIRODUMP_NOT_FOUND);
    //     exit(1);
    // }

    /**
     * Get maximunm number of file descriptors
     */
    struct rlimit rl;
    if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        yah_error("cannot get the maximun number of file descriptors");
        exit(1);
    }

    /**
     * fork and use setsid()
     * In this way, the new process will become a session leader,
     *  and no tty will connect to it.
     */
    // pid_t pid;
    // pid = fork();
    // if(pid < 0) {
    //     yah_error("cannot fork a new subprocess");
    //     exit(1);
    // } else if(pid == 0) { /* parent process */
    //     exit(0);
    // }
    // exit(0);

    /* child process */
    // setsid();


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
        close(fd);
        if(errno == EAGAIN) {
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
