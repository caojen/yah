/**
 * Define all errors' descriptions.
 * All descriptions are type of (const char*). Use yah_error.
 */

#ifndef YEH_ERROR_H
#define YEH_ERROR_H

#include "yah_log.h"

typedef const char* yah_error_type;

#define YAH_E_ALREADY_RUNNINT ((yah_error_type)("yah is already running"))
#define YAH_E_NOT_RUNNING ((yah_error_type)("yah not running"))
#define YAH_E_NO_SUCH_FILE ((yah_error_type)("no such file"))
#define YAH_E_AIRODUMP_NOT_FOUND ((yah_error_type)("airodump not found"))

#define YAH_ERROR(x) yah_error((const char*)(x))

#endif
