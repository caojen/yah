#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <string.h>
#include <stdio.h>

#include "check.hpp"
#include "global.hpp"
#include "log.hpp"
#include "yah_exec.hpp"
#include "sqlite3.h"

int lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence  = SEEK_SET;
    fl.l_len = 0;
    int ret = fcntl(fd, F_SETLK, &fl);

    return ret;
}

namespace yah {
  Check check;

  bool precheck() {
    log << check << "running as root?" << endl;
    if (check_is_root() == false) {
      log << fatal << "... no" << endl;
      goto check_fatal;
    }

    log << check << "system has airodump-ng?" << endl;
    if (check_airodump() == false) {
      log << fatal << "... no" << endl;
      goto check_fatal;
    }

    log << check << "can lock file?" << endl;
    if (check_lockfile() == false) {
      log << fatal << "... no" << endl;
      goto check_fatal;
    }

    log << check << "detect wlan?" << endl;
    if (check_wlan() == false) {
      log << fatal << "... no" << endl;
      goto check_fatal;
    }

    log << check << "init sql..." << endl;
    check_init_sql();
    log << check << "sql init done." << endl;
    return true;

check_fatal:
    log << fatal << "precheck failed" << endl;
    return false;
  }

  bool check_is_root() {
    uid_t id = getuid();
    return id == 0;
  }

  bool check_airodump() {
    return access(config.airodump_path.c_str(), X_OK) == 0;
  }

  bool check_lockfile() {
    int fd = open(config.lockfile.c_str(), O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));
    if(fd < 0) {
      log << "Cannot open " << config.lockfile << ":" << std::strerror(errno) << endl;
      return false;
    }
    if(lockfile(fd) < 0) {
      if(errno == EAGAIN || errno == EACCES) {
          close(fd);
          log << fatal << "lockfile: " << config.lockfile << " has been locked." << endl;
          log << fatal << "another program is running at pid = " << check_get_locking_pid() << endl; 
      } else {
          log << fatal << "Cannot lock " << config.lockfile << ":" << std::strerror(errno) << endl;
      }
      return false;
    }

    long pid = getpid();
    ftruncate(fd, 0);
    char buf[16];
    sprintf(buf, "%ld", pid);
    write(fd, buf, strlen(buf) + 1);

    return true;
  }

  void check_init_sql() {
    const char* ap = "CREATE TABLE if not exists `ap` (id INTEGER PRIMARY KEY AUTOINCREMENT,bssid CHAR(17),comment text,create_time INTEGER not null, is_uploaded int not null default 0);";
    const char* apstation = "CREATE TABLE if not exists `apstation` (id INTEGER PRIMARY KEY AUTOINCREMENT,bssid CHAR (17),station CHAR (17),comment text, create_time INTEGER NOT NULL,is_uploaded int NOT NULL DEFAULT 0);";

    // 初始化全局变量db，并且使用上面的语句应用到db上
    db = DB(config.db);
    sqlite3_open(config.db.c_str(), &db.db);
    sqlite3_exec(db.db, ap, NULL, NULL, NULL);
    sqlite3_exec(db.db, apstation, NULL, NULL, NULL);


    // 清除已经上传的数据
    const char* remove_ap = "DELETE FROM ap WHERE is_uploaded = 1;";
    const char* remove_apstation = "DELETE FROM apstation WHERE is_uploaded = 1;";

    sqlite3_exec(db.db, remove_ap, NULL, NULL, NULL);
    sqlite3_exec(db.db, remove_apstation, NULL, NULL, NULL);
  }

  unsigned check_get_locking_pid() {
    const std::string& lockfile = config.lockfile;
    if(access(lockfile.c_str(), R_OK) != 0) {
      return 0;
    }
    int fd = open(lockfile.c_str(), O_RDONLY);
    if(fd < 0) {
      return 0;
    }

    char pid[12];
    if(read(fd, pid, 12) < 0) {
      return 0;
    }
    return atoi(pid);
  }

  bool check_wlan() {
    char name[64];
    int r = get_airodump_device_name(name);

    device_name = std::string(name);

    log << success << "device name: " << name << endl;
    return r == 0;
  }

  int get_airodump_device_name(char name[64]) {
      char ifconfig[4096];
      int err;
      if((err = yah_exec_shell("ifconfig | grep flags | awk '{print $1}'", ifconfig, 4096)) != 0) {}
      
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
          char ethtool[4096];
          char ethtool_input[4096];
          snprintf(ethtool_input, 4096, "ethtool -i %s | awk 'NR==1{print $2}'", left);
          if((err = yah_exec_shell(ethtool_input, ethtool, 4096)) != 0) {}
          // yah_log("testing %s => %s", left, ethtool);
          int length = strlen(ethtool);
          ethtool[length - 1] = 0;

          if(strcmp(ethtool, config.wlan.c_str()) == 0) {
              snprintf(name, 20, "%s", left);
              return 0;
          }
          right = left = tmp;
      }

      return 1;
  }

}
