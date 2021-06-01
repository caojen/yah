/**
 * used by yah_core.c/.h
 * 
 * functions that dealing with airodump
 */

#ifndef YAH_AIRODUMP_H
#define YAH_AIRODUMP_H

#define YAH_MAX_DEVICE_NAME 20
#define YAH_MAX_IFCONFIG 4096
#define YAH_MAX_ETHTOOL 4096
#define YAH_WLAN "rtl8812au"

/**
 * get the device name that can be used on airodump-ng
 * store into char[] name
 * 
 * return 0 if succeed
 * return other if failed, at this time, name is undefined.
 */
int get_airodump_device_name(char name[YAH_MAX_DEVICE_NAME]);

#include <time.h>
#include "yah_core.h"

struct yah_airodump_apstation {
    char bssid[20];
    char station[20];
    char comment[1024];
    time_t create_time;
};

struct yah_airodump_ap {
    char bssid[20];
    char comment[1024];
    time_t create_time;
};

enum yah_airodump_type {
    apstation = 0,
    ap
};

struct yah_airodump_data {
    enum yah_airodump_type type;
    char bssid[20];
    union {
        struct yah_airodump_apstation apstation;
        struct yah_airodump_ap ap;
    } data ;
};

struct yah_fp_pool_job_arg {
    char line[YAH_CAPTURE_LINE];
    time_t create_time;
};

#endif
