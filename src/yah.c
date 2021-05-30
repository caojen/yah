/**
 * Entry of yah main function.
 */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "yah.h"
#include "yah_const.h"

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
        } else if(strcmp(argv[i], "set") == 0) {
            /* set a key */
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
    printf("Yah -- v1.0\n");
    printf(" Usage: Yah [start | reload | set] <...>\n\n");
    printf(" start: start daemon. If the daemon is started, error.\n");
    printf(" reload: reload settings from config file: %s\n", YAH_CONFFILE);
    printf(" set <key> <value>: set setting. It will update config file.\n");
    printf("   'set' and 'reload' try to change the daemon. It the daemon is not started, error.\n");
}
