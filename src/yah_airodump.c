#include <string.h>
#include <stdio.h>

#include "yah_airodump.h"
#include "yah_log.h"
#include "yah_exec.h"

int
get_airodump_device_name(char name[YAH_MAX_DEVICE_NAME]) {
    char ifconfig[YAH_MAX_IFCONFIG];
    int err;
    if((err = yah_exec_shell("ifconfig | grep flags | awk '{print $1}'", ifconfig, YAH_MAX_IFCONFIG)) != 0) {
        yah_error("get_airodump_device_name error, cannot run ifconfig. exit with code %d", err);
    }
    
    // split with '\n'
    char* left = ifconfig;
    char* right = ifconfig;
    while(*right) {
        while(*right && (*right) != ':') {
            ++right;
        }
        if(*right == 0) {
            break;
        }
        char* tmp = right + 2;      // :\n, right is at ':'
        *right = 0;
        // run `ethtool -i ${left}`
        char ethtool[YAH_MAX_ETHTOOL];
        char ethtool_input[YAH_MAX_ETHTOOL];
        snprintf(ethtool_input, YAH_MAX_ETHTOOL, "ethtool -i %s | awk 'NR==1{print $2}'", left);
        if((err = yah_exec_shell(ethtool_input, ethtool, YAH_MAX_ETHTOOL)) != 0) {
            yah_error("get_airodump_device_name error, cannot run ethtool for %s. exit with code %d", left, err);
        }
        // yah_log("testing %s => %s", left, ethtool);
        int length = strlen(ethtool);
        ethtool[length - 1] = 0;

        if(strcmp(ethtool, YAH_WLAN) == 0) {
            snprintf(name, YAH_MAX_DEVICE_NAME, "%s", left);
            return 0;
        }
        right = left = tmp;
    }

    return 1;
}