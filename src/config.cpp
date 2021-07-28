#include <fstream>
#include <string>
#include <array>

#include "config.hpp"
#include "log.hpp"
#include "global.hpp"

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
  yah::log << ctime << "(Re)loading from file: " << this->config_file << endl;
  
  pthread_mutex_lock(&this->config_mutex);

  std::ifstream ifs;
  ifs.open(this->config_file, std::ios::in);

  if (ifs.is_open() == false) {
    std::cout << "Unknown File: " << this->config_file;
  }

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
    set_config_value(ifs, key, sender_await);
  }

  pthread_mutex_unlock(&this->config_mutex);

  Config::show(*this);
}

void Config::show(const Config& config) {
  show_config_value(yah::log, config, config_file);
  show_config_value(yah::log, config, remote_address);
  show_config_value(yah::log, config, remote_port);
  show_config_value(yah::log, config, remote_ap);
  show_config_value(yah::log, config, remote_apstation);
  show_config_value(yah::log, config, device);
  show_config_value(yah::log, config, db);
  show_config_value(yah::log, config, log);
  show_config_value(yah::log, config, log_to_std);
  show_config_value(yah::log, config, num_formatter);
  show_config_value(yah::log, config, num_checker);
  show_config_value(yah::log, config, num_sender);
  show_config_value(yah::log, config, num_send_msg);
  show_config_value(yah::log, config, msg_queue_size);
  show_config_value(yah::log, config, airodump_path);
  show_config_value(yah::log, config, airodump_name);
  show_config_value(yah::log, config, lockfile);
}
