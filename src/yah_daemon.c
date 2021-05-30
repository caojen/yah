#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include "yah_const.h"
#include "yah_log.h"
#include "yah_daemon.h"

int
yah_daemonize(void) {

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
        }
    }

    // write self.pid to fd
    long pid = getpid();
    ftruncate(fd, 0);
    char buf[16];
    sprintf(buf, "%ld", pid);
    write(fd, buf, strlen(buf) + 1);
    yah_log("lockfile: ok");
    return YAH_DAEMON_RUNNING;
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
