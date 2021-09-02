#include "white_list.hpp"
#include "request.hpp"
#include "global.hpp"

#include <thread>
#include <boost/json/src.hpp>
#include <boost/beast/core/string.hpp>

yah::WhiteList::WhiteList() {
  std::thread thread([this] () {
    ot::Request<ot::http, ot::post> request;
    request.set_host("fscc.in.3322.org");
    request.set_port(36675);
    request.set_target("/pidc/layoutinfo/listAll.action");
    request.send();

    auto body = request.get_response_body();

    boost::beast::error_code ec;
    boost::json::value jv = boost::json::parse(body, ec);
    if(ec) {
      yah::log << ctime << "white list error parsing response: " << ec << endl;
    } else {
      auto data = jv.as_object()["data"].as_array();
      std::list<std::string> new_white_data;
      for(unsigned i = 0; i < data.size(); ++i) {
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
