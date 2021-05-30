#include <stdio.h>
#include <stdlib.h>

#include "yah_log.h"

void
yah_log(const char* fmt, ...) {
    unimplemented();
}

void
yah_warn(const char* fmt, ...) {
    unimplemented();
}

void
yah_error(const char* fmt, ...) {
    unimplemented();
}

void
unimplemented() {
    sprintf(stderr, "Unimplemented!");
    exit(1);
}
