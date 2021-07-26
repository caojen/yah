#pragma once

#include <string>

#include "dict.hpp"

namespace yah {
  class Response;

  class Request {
    public:
      Request();

      // 指定域名，然后找ip
      Request& host(const std::string& h);
      // 指定端口，如果不指定，使用80
      Request& port(size_t p);
      // 指定ip，如果ip不指定，那么根据指定的host来查找ip。
      // 相反，如果host不指定，而指定了ip，那么将ip当作host
      Request& ip(const std::string& i);

      Request& path(const std::string& pa);

      // 设置一个请求头
      // Host, Connection, Content-Length会被动态生成
      Request& header(const std::string& key, const Value& value);

      // 设置body，并将Content-Type设置为txt
      Request& body(const std::string& body);
      // 设置body，并将Content-Type设置为Application/json
      Request& body(const Json& json);
      // 发送这个请求
      Response post();
    private:
      std::string __host = "";
      size_t      __port = 80;
      std::string __ip   = "";
      std::string __path = "";
      Json        __headers = Json();
      std::string __body = "";
  };

  class Response {
    public:
      // 根据tcp文本，自动生成状态吗
      Response(const std::string& text);
      Response();
      // 返回状态码
      size_t status() const;
      // 返回TCP文本
      std::string tcp_text() const;
    private:
      size_t code = 100;
      std::string text = "";
  };
}
