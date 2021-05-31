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

#endif
