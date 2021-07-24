#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "yah_log.h"
#include "yah_const.h"
#include "yah_config.h"

int log_level = YAH_LOG_LEVEL_LOG;
int rpworkers = 1;
int fpworkers = 1;

int device_number;

FILE* YAH_FILENO_LOG = 0;
FILE* YAH_FILENO_WARN = 0;
FILE* YAH_FILENO_ERROR = 0;

// A mutex is used for reloading config file.
// The thread that want to `modify` config should try to get the mutex.
pthread_mutex_t yah_config_mutex = PTHREAD_MUTEX_INITIALIZER;

int
yah_reload_config() {
    if(pthread_mutex_lock(&yah_config_mutex) != 0) {
        perror("open config file");
        return -1;
    }
    
    // open config file in 'r' mode:

    FILE* fp = fopen(YAH_CONFFILE, "r");
    if(fp == NULL) {
        perror("open config file");
        return -1;
    }
    char line[YAH_MAXLINE];
    while(fgets(line, YAH_MAXLINE, fp) != NULL) {
        char* ptr = line;
        while(*ptr == ' ') {
            ++ptr;
        }
        if(*ptr == '\n') {
            // empty line
            continue;
        }
        if(*ptr == '#') {
            // try skiping to next line
            int length = strlen(line);
            if(length >= YAH_MAXLINE - 1) {
                char ch;
                while((ch = getc(fp)) != EOF) {
                    if(ch == '\n') {
                        break;
                    }
                }
            }
        } else {
            char key[YAH_MAXLINE];
            int value;
            sscanf(line, "%s %d", key, &value);
            if(yah_set_config_without_mutex(key, value) != 0) {
                yah_warn("Config warning. No such key: %s", key);
            }
        }
    }

    pthread_mutex_unlock(&yah_config_mutex);
    return 0;
}

int
yah_set_config(const char* key, int value) {
    pthread_mutex_lock(&yah_config_mutex);

    int ret = yah_set_config_without_mutex(key, value);

    pthread_mutex_unlock(&yah_config_mutex);

    return ret;
}

int
yah_set_config_without_mutex(const char* key, int value) {
    int ret = -1;

    if(strcmp(key, "log_level") == 0) {
        log_level = value;
        ret = 0;
    } else if(strcmp(key, "rpworkers") == 0) {
        rpworkers = value;
        ret = 0;
    } else if(strcmp(key, "fpworkers") == 0) {
        fpworkers = value;
        ret = 0;
    } else if(strcmp(key, "device_number") == 0) {
        device_number = value;
        ret = 0;
    }

    return ret;
}
