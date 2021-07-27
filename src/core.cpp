#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>

#include "core.hpp"
#include "global.hpp"
#include "yah_pty.hpp"
#include "formatter.hpp"
#include "http.hpp"
#include "encode.hpp"

namespace yah {
  static inline void init_ap_cache() {
    ap_cache = Cache<std::string>(config.num_cache_max, config.ap_cache_timeout);
  }
  static inline void init_apstation_cache() {
    apstation_cache = Cache<std::string>(config.num_cache_max, config.apstation_cache_timeout);
  }
  static inline void init_formatter() {
    formatter = new ThreadPool(config.num_formatter);
  }
  static inline void init_checker() {
    checker = new ThreadPool(config.num_checker);
  }
  static inline void init_sender() {
    auto func = [](const std::vector<std::unique_ptr<AirodumpData>>& vec) {
      log << "[Sender] " << yah::ctime << vec.size() << endl;
      std::vector<Json> ap;
      std::vector<Json> apstation;
      for(auto& item: vec) {
        if(item->is_ap()) {
          ap.push_back(item->serialize());
        } else if(item->is_apstation()) {
          apstation.push_back(item->serialize());
        }
      }
      if(!ap.empty()) {
        std::string data = Json::serialize(ap);
        log << yah::ctime << "[Sender Ap Body] " << data << endl;

        Encode encoder(data);
        std::string body = encoder.encode();
        log << yah::ctime << "[Sender Ap Encoded] " << body << endl;
        Json json; json.set("data", body);
        Response response = Request()
          .host(config.remote_address)
          .port(config.remote_port)
          .path(config.remote_ap)
          .body(json.serialize())
          .header("Content-Type", std::string("Application/json"))
          .post();
        log << yah::ctime << "[Sender] Send Done. " << response.status() << endl;
      }
      if(!apstation.empty()) {
        std::string data = Json::serialize(apstation);
        log << yah::ctime << "[Sender ApStation Body] " << data << endl;

        Encode encoder(data);
        std::string body = encoder.encode();
        log << yah::ctime << "[Sender Ap Encoded] " << body << endl;
        Json json; json.set("data", body);

        Response response = Request()
          .host(config.remote_address)
          .port(config.remote_port)
          .path(config.remote_apstation)
          .body(json.serialize())
          .header("Content-Type", std::string("Application/json"))
          .post();
        log << yah::ctime << "[Sender] Send Done. " << response.status() << endl;
      }
    };
    sender = new AutoPool<AirodumpData>(config.num_sender, config.num_send_msg, func, config.sender_await);
  }
}

namespace yah {
  void core_start() {
    init_ap_cache();
    init_apstation_cache();
    init_formatter();
    init_checker();
    init_sender();

    log << success << "init done" << endl;

    int airodump_fd = run_airodump();
    log << success << "airodump running at file socket = " << airodump_fd << endl;

    fd_set input_set;
    FD_ZERO(&input_set);
    FD_SET(airodump_fd, &input_set);
    struct timeval timeout; timeout.tv_sec = 15; timeout.tv_usec = 0; // select 允许超时15s
    while(1) {
      if(select(1, &input_set, NULL, NULL, &timeout) == -1) {
        // 超时检查
        log << warn << "airodump select timeout. Checking..." << endl;
        int status = 0;
        if(waitpid(airodump_pid, &status, WNOHANG) == 0) {
          log << warn << "ok. still running" << endl;
        } else {
          log << fatal << "airodump-ng exited. Abort";
          abort();
        }
      } else {
        // 允许读:
        char ch;
        std::string line = "";
        // 一直读，读到换行符为止
        while(1) {
          while(read(airodump_fd, &ch, 1) == -1 && errno == EINTR);
          line.append(std::string(1, ch));
          if(ch == '\n') {
            break;
          }
        }
        if(line.size() < 10) {
          continue;
        }

        std::unique_ptr<Job> ptr(new Formatter(line));
        formatter->push(ptr);
      }
    }
  }

  int run_airodump() {
    int fd = 0;
    char slave_name[MAX_PTYNAME];
    struct winsize wsize;
    wsize.ws_row = YAH_PTY_ROWS;
    wsize.ws_col = YAH_PTY_COLS;

    pid_t pid = yah_pty_fork(&fd, slave_name, MAX_PTYNAME, NULL, &wsize);
    airodump_pid = pid;

    if(pid == 0) {
      // 子进程
      execl(config.airodump_path.c_str(), config.airodump_name.c_str(), device_name.c_str(), "-C", "2412-2472,5180-5825", "-f", "10", "--berlin", "3", NULL);
    } else {
      return fd;
    }
  }
}
