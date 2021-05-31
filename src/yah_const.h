/**
 * Define all constants that yah will use.
 * Not like 'config', constants are built-in the program.
 */

#ifndef YAH_CONST_H
#define YAH_CONST_H

#include <unistd.h>
#include <sys/stat.h>

#define DEBUG 1

#if DEBUG

#define YAH_LOCKFILE "/Users/caojen/Documents/youarehere2/yah/yah.pid"
#define YAH_CONFFILE "/Users/caojen/Documents/youarehere2/yah/yah.conf"

#else

#define YAH_LOCKFILE "/var/run/yah.pid"
#define YAH_CONFFILE "/etc/yah.conf"

#endif

#define YAH_LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define YAH_MAXLINE 2048

// used in daemon
#define YAH_DAEMON_RUNNING 1        // the daemon is running
#define YAH_DAEMON_NOTRUNNING 0     // the daemon is not running
#define YAH_DAEMON_ERROR_ING 1      // ignore error when log_error

// used in checking airodump
#define YAH_AIRODUMP "/usr/bin/airodump-ng"
#define YAH_AIRODUMP_EXISTS 1       // the airodump can be found
#define YAH_CHECK_AIRODUMP 0

// log file path
#if DEBUG
    #define YAH_LOGFILE_LOG "/Users/caojen/Documents/youarehere2/yah/yah.log"
    #define YAH_LOGFILE_WARN "/Users/caojen/Documents/youarehere2/yah/yah.warn"
    #define YAH_LOGFILE_ERROR "/Users/caojen/Documents/youarehere2/yah/yah.error"
#else
    #define YAH_LOGFILE_LOG "yah.log"
    #define YAH_LOGFILE_WARN "yah.warn"
    #define YAH_LOGFILE_ERROR "yah.error"
#endif

// for root testing
#define YAH_RUNNING_AS_ROOT 1
#define YAH_ROOT_REQUIRED 0

#endif
