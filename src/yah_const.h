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
    #define YAH_LOCKFILE "yah.pid"
    #define YAH_CONFFILE "yah.conf"
#else
    #define YAH_LOCKFILE "/var/run/yah.pid"
    #define YAH_CONFFILE "/etc/yah.conf"
#endif

#define YAH_LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define YAH_MAXLINE 1024

// used in daemon
#define YAH_DAEMON_RUNNING 1        // the daemon is running
#define YAH_DAEMON_NOTRUNNING 0     // the daemon is not running
#define YAH_DAEMON_ERROR_ING 1      // ignore error when log_error

// used in checking airodump
#define YAH_AIRODUMP "/bin/airodump"
#define YAH_AIRODUMP_EXISTS 1       // the airodump can be found
#endif
