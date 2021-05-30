/**
 * Entry of yah main function.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "yah.h"

int
main(int argc, char* argv) {
    if(argc <= 1) {
        /* no argument supplied. print usage. */
        yah_usage();
        exit(EINVAL);
    }

    /* test all arguments. */
    for(int i = 1; i < argc; i++) {

    }
}

void
yah_usage(void) {

}