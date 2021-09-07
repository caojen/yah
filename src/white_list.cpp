#include "white_list.hpp"
#include "request.hpp"
#include "global.hpp"
#include "validate.hpp"
#include "yah_exec.hpp"

#include <thread>
#include <boost/json/src.hpp>
#include <boost/beast/core/string.hpp>

yah::WhiteList::WhiteList() {
  std::thread thread([this] () {
    ot::Request<ot::http, ot::post> request;
    request.set_host(yah::config.remote_address);
    request.set_port(yah::config.remote_port);
    request.set_target("/pidc/layoutinfo/listAll.action");
    request.send();

    auto body = request.get_response_body();

    if(!body.empty()) {

      boost::beast::error_code ec;
      boost::json::value jv = boost::json::parse(body, ec);
      if (ec) {
        yah::log << ctime << "white list error parsing response: " << ec
                 << endl;
      } else {
        auto data = jv.as_object()["data"].as_array();
        std::list<std::string> new_white_data;
        for (unsigned i = 0; i < data.size(); ++i) {
          auto item = data[i].as_object();
          auto mac = std::string(item["mac"].as_string().c_str());
          new_white_data.push_back(mac);
          yah::log << ctime << "get white list result: " << mac << endl;
        }
        {
          std::lock_guard<std::mutex> lk(this->mutex);
          this->white_data.clear();
          this->white_data = std::move(new_white_data);
        }
      }
    }

    std::this_thread::sleep_for(std::chrono::minutes(5));
  });

  thread.detach();
}
bool yah::WhiteList::has_data(const std::string& s) {
  std::lock_guard<std::mutex> lk(this->mutex);
  bool ret = false;
  for(auto iter = this->white_data.begin(); iter != this->white_data.end(); ++iter) {
    if(boost::beast::iequals(*iter, s)) {
      ret = true;
      break;
    }
  }

  return ret;
}

yah::WhiteList* yah::whiteList = nullptr;

void yah::Validate::check() {


  ot::Request<ot::http, ot::get> request;
  request.set_host(yah::config.remote_address);
  request.set_port(yah::config.remote_port);
  request.set_target("/pidc/equipment/listAll.action");
  request.send();
  auto body = request.get_response_body();
  if(!body.empty()) {
    yah::log <<yah::ctime << "Validate get data " << body << endl;
    bool check_pass = false;

    boost::beast::error_code ec;
    boost::json::value jv = boost::json::parse(body, ec);
    if(ec) {
      yah::log << ctime << "Validate parse error " << ec << endl;
    } else {
      auto data = jv.as_object()["data"].as_array();
      std::map<unsigned, std::string> devices;
      for(unsigned i = 0; i < data.size(); ++i) {
        auto item = data[i].as_object();
        auto mac = std::string(item["mac"].as_string().c_str());
        auto mobile = atoi(item["mobile"].as_string().c_str());
        std::string m;
        for(auto& ch: mac) {
          if(ch >= 'A' && ch <= 'Z') {
            m.append(std::string(1, ch - 'A' + 'a'));
          } else {
            m.append(std::string(1, ch));
          }
        }
	      devices[mobile] = m;
      }
      for(auto& item: devices) {
        yah::log << ctime << "Validate get item: " << item.first << "=>" << item.second << endl;
      }
      yah::log << ctime << "Validate get this device: " << config.device << endl;
      auto iter = devices.find(config.device);
      if(iter == devices.end()) {
        yah::log << ctime << "Validate Not found" << endl;
      } else {
        yah::log << ctime << "Validate found, should be:" << iter->second << endl;
        char output[10240] = { 0 };
        yah_exec_shell("ifconfig", output, 10240);
        std::string s_output = std::string(output);
        if(s_output.find(iter->second) != std::string::npos) {
          check_pass = true;
        }
      }
      yah::log << ctime << "Validate check return: " << check_pass << endl;
      if(!check_pass) {
        yah::log << ctime << "Validate failed. will run fatal()" << endl;
       //  Validate::fatal();
      }
    }
  }
}
