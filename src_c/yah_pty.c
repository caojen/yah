#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "yah_pty.h"
#include "yah_log.h"

int
yah_ptym_open(char* pts_name, int pts_namesz) {
    char *ptr;
    int fdm, err;

    if((fdm = posix_openpt(O_RDWR)) < 0) {
        return -1;
    }
    if(grantpt(fdm) < 0) {
        goto errout;
    }
    if(unlockpt(fdm) < 0) {
        goto errout;
    }
    if((ptr = ptsname(fdm)) == NULL) {
        goto errout;
    }

    strncpy(pts_name, ptr, pts_namesz);
    pts_name[pts_namesz - 1] = 0;
    return fdm;

errout:
    err = errno;
    close(fdm);
    errno = err;
    return -1;
}

int
yah_ptys_open(char* pts_name) {
    int fds;
    if((fds = open(pts_name, O_RDWR)) < 0) {
        return -1;
    }
    return fds;
}

pid_t
yah_pty_fork(int* ptrfdm, char* slave_name, int slave_namesz,
    const struct termios* slave_termios,
    const struct winsize* slave_winsize) {
    int fdm, fds;
    pid_t pid;
    char pts_name[MAX_PTYNAME];

    if((fdm = yah_ptym_open(pts_name, sizeof(pts_name))) < 0) {
        yah_quit("cannot open master pty: %s, error %d: %s", pts_name, fdm, strerror(errno));
    }
    yah_log("pty_fork done. fdm=%d, pts_name=%s", fdm, pts_name);

    if(slave_name != NULL) {
        strncpy(slave_name, pts_name, slave_namesz);
        slave_name[slave_namesz - 1] = 0;
    }

    if((pid = fork()) < 0) {
        yah_quit("cannot fork in pty_fork");
    } else if(pid == 0) {
        yah_log("blocking the subprocess");
        /* child process */
        if(setsid() < 0) {
            yah_quit("pty_fork.child: setsid error");
        }

        if((fds = yah_ptys_open(pts_name)) < 0) {
            yah_quit("pty_fork.child: cannot open ptys");
        }
        
        yah_log("pty_fork.child: open success with fds=%d, %s", fds, pts_name);
        close(fdm);
       
        if(slave_termios != NULL) {
            if(tcsetattr(fds, TCSANOW, slave_termios) < 0) {
                yah_error("pty_fork.child: error when setting termios");
            }
        }
        
        if(slave_winsize != NULL) {
            if(ioctl(fds, TIOCSWINSZ, slave_winsize) < 0) {
                yah_error("pty_fork.child: error when setting winsize");
            }
        }
        /* slave(fds) should become the stdin, stdout, stderr of child */
        if(dup2(fds, STDIN_FILENO) != STDIN_FILENO) {
            yah_error("pty_fork.child cannot bind stdin");
        }
        if(dup2(fds, STDOUT_FILENO) != STDOUT_FILENO) {
            yah_error("pty_fork.child connot bind stdout");
        }
        if(dup2(fds, STDERR_FILENO) != STDERR_FILENO) {
            yah_error("pty_fork.child cannot build stderr");
        }

        if(fds != STDIN_FILENO && fds != STDOUT_FILENO && fds != STDERR_FILENO) {
            yah_log("pty_fork.child close fds = %d", fds);
            close(fds);
        }
        return 0; // child process return 0
    } else {
        /* parent process */
        yah_log("pty.child.pid = %d", pid);
        *ptrfdm = fdm; // return the fdm in ptrfdm
        return pid;    // return the child process' pid
    }
    return 1;
}
