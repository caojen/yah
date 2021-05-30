/**
 * configs that can be used in yah.
 * 
 * all configs are type of int.
 */

#ifndef YAH_CONFIG_H
#define YAH_CONFIG_H

#include <stdio.h>

extern int log_level;
extern FILE* YAH_FILENO_LOG;
extern FILE* YAH_FILENO_WARN;
extern FILE* YAH_FILENO_ERROR;


/**
 * reload config from CONFFILE
 * return 0 if success
 * return not 0 if failed
 */
int yah_reload_config();

/**
 * set config with key and new value
 * return 0 if success
 * return not 0 if failed
 * 
 * failed means no such key.
 */
int yah_set_config(const char* key, int value);

/**
 * same as `yah_set_config`
 * but this function will not try to get config mutex
 */
int yah_set_config_without_mutex(const char* key, int value);

#endif
