#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "yah_log.h"
#include "yah_const.h"
#include "yah_config.h"

void
yah_log_doit(FILE* fp, const char* fmt, va_list ap) {
    char buf[YAH_MAXLINE];
    int length = vsnprintf(buf, YAH_MAXLINE - 2, fmt, ap);
    buf[length] = '\n';
    buf[length + 1] = 0;

    fputs(buf, fp);
}

void
yah_log(const char* fmt, ...) {
    if(log_level <= YAH_LOG_LEVEL_LOG) {
        va_list ap;

        va_start(ap, fmt);
        yah_log_doit(stdout, fmt, ap);
    }
}

void
yah_warn(const char* fmt, ...) {
    if(log_level <= YAH_LOG_LEVEL_WARN) {
        va_list ap;

        va_start(ap, fmt);
        yah_log_doit(stdout, fmt, ap);
    }
}

void
yah_error(const char* fmt, ...) {
    if(log_level <= YAH_LOG_LEVEL_ERROR) {
        va_list ap;

        va_start(ap, fmt);
        yah_log_doit(stderr, fmt, ap);
    }
    if(!YAH_DAEMON_ERROR_ING) {
        exit(1);
    }
}

void
unimplemented() {
    fprintf(stderr, "Unimplemented!\n");
    exit(1);
}
