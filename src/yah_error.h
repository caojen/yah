/**
 * Define all errors' descriptions.
 * All descriptions are type of (const char*). Use yah_error.
 */

#ifndef YEH_ERROR_H
#define YEH_ERROR_H

typedef const char* yah_error;

#define YAH_E_NOT_RUNNING ((yah_error)("yah not running"))
#define YAH_E_NO_SUCH_FILE ((yah_error)("no such file"))

#endif