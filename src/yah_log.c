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
yah_log_hex(const char* bytes, unsigned bysz) {
    if(log_level <= YAH_LOG_LEVEL_LOG) {
        for(int i = 0; i < bysz; i++) {
            fprintf(YAH_FILENO_LOG, "%x ", bytes[i]);
        }
        fprintf(YAH_FILENO_LOG, "\n");
    }
}

void
unimplemented() {
    yah_error("Unimplemented!");
    daemon_exit();
    exit(1);
}

void
unreachable() {
    fprintf(stderr, "thread has reached unreachable code!\n");
    yah_error("thread has reached unreachable code!");
    daemon_exit();
    exit(1);
}

void
rename_logs() {
    time_t t = time(NULL);
    char newname[256] = { 0 };
    if(access(YAH_LOGFILE_LOG, R_OK) == 0) {
        sprintf(newname, "%s.%ld", YAH_LOGFILE_LOG, t);
        rename(YAH_LOGFILE_LOG, newname);
    }
    if(access(YAH_LOGFILE_WARN, R_OK) == 0) {
        sprintf(newname, "%s.%ld", YAH_LOGFILE_WARN, t);
        rename(YAH_LOGFILE_WARN, newname);
    }
    if(access(YAH_LOGFILE_ERROR, R_OK) == 0) {
        sprintf(newname, "%s.%ld", YAH_LOGFILE_ERROR, t);
        rename(YAH_LOGFILE_ERROR, newname);
    }
}
