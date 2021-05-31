#include <stdlib.h>
#include <fcntl.h>

#include "yah_pty.h"

int
yah_ptym_open(char* pts_name, int pts_namesz) {
    char *ptr;
    int fdm, err;

    if((fdm = posix_openpt(O_RDWR)) < 0) {
        return -1;
    }
    if(grantpt(fdm) < 0) {

    }
}