#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "yah_pty.hpp"

namespace yah {

    int yah_ptym_open(char* pts_name, int pts_namesz) {
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

        if((fdm = yah_ptym_open(pts_name, sizeof(pts_name))) < 0) {}

        if(slave_name != NULL) {
            strncpy(slave_name, pts_name, slave_namesz);
            slave_name[slave_namesz - 1] = 0;
        }

        if((pid = fork()) < 0) {
        } else if(pid == 0) {
            /* child process */
            if(setsid() < 0) {}

            if((fds = yah_ptys_open(pts_name)) < 0) {}
            
            close(fdm);
        
            if(slave_termios != NULL) {
                if(tcsetattr(fds, TCSANOW, slave_termios) < 0) {}
            }
            
            if(slave_winsize != NULL) {
                if(ioctl(fds, TIOCSWINSZ, slave_winsize) < 0) {}
            }
            /* slave(fds) should become the stdin, stdout, stderr of child */
            if(dup2(fds, STDIN_FILENO) != STDIN_FILENO) {}
            if(dup2(fds, STDOUT_FILENO) != STDOUT_FILENO) {}
            if(dup2(fds, STDERR_FILENO) != STDERR_FILENO) {}

            if(fds != STDIN_FILENO && fds != STDOUT_FILENO && fds != STDERR_FILENO) {
                close(fds);
            }
            return 0; // child process return 0
        } else {
            /* parent process */
            *ptrfdm = fdm; // return the fdm in ptrfdm
            return pid;    // return the child process' pid
        }
        return 1;
    }

}