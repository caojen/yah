/**
 * log functions that yah uses.
 * 
 * all functions call fprintf, so a va_list is used to accept arguments.
 * see also: printf(1), printf(3)
 * 
 * all functions return (void).
 * 
 * There are 3 log levels:
 *  1 = log, STDOUT
 *  2 = warn, STDOUT
 *  3 = error, STDERR
 * 
 * log level is retrived from config file.
 * 
 * because `typedef const char* yah_error`(defined at yah_error), yah_error can be accepted.
 */

#ifndef YAH_LOG_H
#define YAH_LOG_H

#define YAH_LOG_LEVEL_LOG 1
#define YAH_LOG_LEVEL_WARN 2
#define YAH_LOG_LEVEL_ERROR 3

void yah_log(const char* fmt, ...);
void yah_warn(const char* fmt, ...);
void yah_error(const char* fmt, ...);
void yah_log_hex(const char* bytes, unsigned bysz);

#include <stdlib.h>
#define yah_log_string_hex(bytes) yah_log_hex((bytes), strlen((bytes)));

#include "yah_daemon.h"

#define yah_quit(...) do {                          \
    yah_error(__VA_ARGS__);                         \
    yah_error("yah_quit called, exit...");          \
    yah_log("yah_quit called, exit...");            \
    yah_warn("yah_quit called, exit...");           \
    daemon_exit();                                  \
} while(0)

/**
 * This function means something is not implemented.
 * It is an error, and stop the process with exit code 1.
 */
void unimplemented();

/**
 * This function means threads reaches unexpected code.
 * It is an error, stop the process with exit code 1.
 */
void unreachable();

/**
 * store old logfile with other name
 */
void rename_logs();

#endif
