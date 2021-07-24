#pragma once

#include <iostream>
#include <string>
#include <pthread.h>

namespace yah {
  class Config {
    std::string   config_file           =     "";         // 本次使用的配置文件路径
    pthread_mutex_t config_mutex;                       // 配置写锁

    public:
      std::string   remote_address      =     "";         // 用于查找ip的域名
      unsigned      remote_port         =     0;          // 该服务器的开放端口
      std::string   remote_ap           =     "";         // ap所在子目录
      std::string   remote_apstation    =     "";         // apstation所在子目录

      unsigned      device              =     0;          // 该设备号

      std::string   db                  =     "";         // 本地数据库文件位置
      std::string   log                 =     "";         // log文件位置
      bool          log_to_std          =     false;      // 是否将log内容输出到标准输出中

      unsigned      num_formatter       =     10;         // 处理行的线程数量，将行格式化成结构体，不能通过reload配置，下同
      unsigned      num_checker         =     10;         // 检查结构体是否应该被保存，线程数量
      unsigned      num_sender          =     1;          // 发送者的数量
      unsigned      num_send_msg        =     100;        // 发送者发送消息时的最大数量
      unsigned      msg_queue_size      =     10000;      // 消息队列大小

      Config();
      Config(const std::string& filename);

      // 重载配置文件
      void reload();
  };
}

#define set_config_value(_ifs, _key, _target) do { \
  if (_key == #_target) {                          \
    ifs >> this->_target;                          \
  }                                                \
} while(0)
