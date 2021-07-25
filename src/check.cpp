#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>

#include "check.hpp"
#include "global.hpp"
#include "log.hpp"

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

    return true;
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
}
