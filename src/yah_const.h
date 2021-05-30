/**
 * Define all constants that yah will use.
 */

#ifndef YAH_CONST_H
#define YAH_CONST_H

#include <unistd.h>
#include <sys/stat.h>

#define YAH_LOCKFILE "/var/run/yah.pid"
#define YAH_LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define YAH_CONFFILE "/etc/yah.conf"
#endif
