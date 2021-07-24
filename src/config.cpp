#include <fstream>
#include <string>
#include <array>

#include "config.hpp"

using namespace yah;

Config::Config() {
  // pthread_mutex_init(&this->config_mutex, NULL);
}

Config::Config(const std::string& filename) {
  this->config_file = filename;
  pthread_mutex_init(&this->config_mutex, NULL);
  this->reload();
}

template<std::size_t N>
inline static bool array_contains(
  const std::array<std::string, N>& array,
  const std::string& str
) {
  for (auto& s: array) {
    if (s == str) {
      return true;
    }
  }
  return false;
}

void Config::reload() {
  pthread_mutex_lock(&this->config_mutex);

  std::ifstream ifs;
  ifs.open(this->config_file, std::ios::in);

  static std::array<std::string, 5> as_string = {
    "remote_address",
    "remote_ap",
    "remote_apstation",
    "db",
    "log"
  };

  std::string key;
  while (ifs >> key) {
    set_config_value(ifs, key, remote_address);
    set_config_value(ifs, key, remote_port);
    set_config_value(ifs, key, remote_ap);
    set_config_value(ifs, key, remote_apstation);
    set_config_value(ifs, key, device);
    set_config_value(ifs, key, db);
    set_config_value(ifs, key, log);
    set_config_value(ifs, key, log_to_std);
    set_config_value(ifs, key, num_formatter);
    set_config_value(ifs, key, num_checker);
    set_config_value(ifs, key, num_sender);
    set_config_value(ifs, key, num_send_msg);
    set_config_value(ifs, key, msg_queue_size);
  }

  pthread_mutex_unlock(&this->config_mutex);
}
