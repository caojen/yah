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
        if(strcmp(argv[i], "start") == 0) {
            /* start as daemon */
        } else if(strcmp(argv[i], "reload") == 0) {
            /* reload config */
            /* send SIGHUP to daemon */
        } else if(strcmp(argv[i], "reset") == 0) {
            /* reset a key */
            /* set the key to config */
            /* then send SIGHUP to daemon */
        } else {
            yah_usage();
            exit(EINVAL);
        }
    }
}

void
yah_usage(void) {

}