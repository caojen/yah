#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "yah_log.h"
#include "yah_const.h"
#include "yah_config.h"

void
yah_log_doit(FILE* fp, const char* fmt, va_list ap) {
    if(fp != NULL) {
        
        time_t t = time(NULL);
        struct tm* local = localtime(&t);

        char time_str[YAH_MAXLINE];
        strftime(time_str, YAH_MAXLINE, "[%c] ", local);

        char buf[YAH_MAXLINE];
        int length = vsnprintf(buf, YAH_MAXLINE - 2, fmt, ap);
        buf[length] = '\n';
        buf[length + 1] = 0;
        fputs(time_str, fp);
        fputs(buf, fp);
        fflush(fp);
    }
}

void
yah_log(const char* fmt, ...) {
    if(log_level <= YAH_LOG_LEVEL_LOG) {
        va_list ap;

        va_start(ap, fmt);
        yah_log_doit(YAH_FILENO_LOG, fmt, ap);
    }
}

void
yah_warn(const char* fmt, ...) {
    if(log_level <= YAH_LOG_LEVEL_WARN) {
        va_list ap;

        va_start(ap, fmt);
        char buf[YAH_MAXLINE];
        sprintf(buf, "warn: %s", fmt);
        yah_log_doit(YAH_FILENO_WARN, buf, ap);
    }
}

void
yah_error(const char* fmt, ...) {
    if(log_level <= YAH_LOG_LEVEL_ERROR) {
        va_list ap;

        va_start(ap, fmt);
        char buf[YAH_MAXLINE];
        sprintf(buf, "fatal: %s", fmt);
        yah_log_doit(YAH_FILENO_ERROR, buf, ap);
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
