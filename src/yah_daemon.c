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

    int isrunning = check_daemon_running();
    if(isrunning == YAH_DAEMON_RUNNING) {
        YAH_ERROR(YAH_E_ALREADY_RUNNINT);
        exit(1);
    }

    int has_airodump = check_airodump_exists();
    if(has_airodump != YAH_AIRODUMP_EXISTS) {
        YAH_ERROR(YAH_E_AIRODUMP_NOT_FOUND);
        exit(1);
    }

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
    sleep(19);
    return YAH_DAEMON_RUNNING;
}

int
check_airodump_exists(void) {
    // test if we can access YAH_AIRODUMP with X_OK
    int can_access = access(YAH_AIRODUMP, X_OK);
    int ret = -1;
    if(can_access) {
        ret = YAH_AIRODUMP_EXISTS;
    } else {
        ret = !YAH_AIRODUMP_EXISTS;
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
