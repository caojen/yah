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

#endif
