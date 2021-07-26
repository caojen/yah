#include <sstream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <cstring>

#include "http.hpp"
#include "global.hpp"

namespace yah {
  Request::Request() {}

  Request& Request::host(const std::string& h) {
    this->__host = h;
    return *this;
  }

  Request& Request::port(size_t p) {
    this->__port = p;
    return *this;
  }

  Request& Request::ip(const std::string& ip) {
    this->__ip = ip;
    this->__host = ip;
    return *this;
  }

  Request& Request::path(const std::string& path) {
    this->__path = path;
    return *this;
  }

  Request& Request::header(const std::string& key, const Value& value) {
    this->__headers.set(key, value);
    return *this;
  }

  Request& Request::body(const std::string& body) {
    this->__body = body;
    return this->header("Content-Type", "text/plain");
  }

  Request& Request::body(const Json& json) {
    this->__body = json.serialize();
    return this->header("Content-Type", "Application/json");
  }

  Response Request::post() {
    std::ostringstream ostr("");
    auto tcp_break = "\r\n";
    ostr << "POST " << this->__path << " HTTP/1.0" << tcp_break;

    this->__headers.set("Connection", "close");
    for(auto &pair: this->__headers.items) {
      ostr << pair.first.serialize() << ": " << pair.second.serialize() << tcp_break;
    }

    ostr << tcp_break;
    ostr << this->__body << tcp_break;

    std::string text = ostr.str();

    log << ctime << "[Http] Generate Body:" << endl << text << endl;

    // send text to ip this->__ip;

    // connect
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    serv_addr.sin_family = PF_INET;
    inet_aton(this->__ip.c_str(), &serv_addr.sin_addr);
    serv_addr.sin_port = htons(this->__port);

    std::string r = "";
    char response[1024] = { 0 };
    // send
    if(connect(fd, (struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0) {
      log << yah::ctime << "[Http] socker.connect error" << endl;
      goto error;
    }
    if(write(fd, text.c_str(), text.size()) == -1) {
      log << yah::ctime << "[Http] socket.write error" << endl;
      goto error;
    }
    // receive response
    read(fd, response, 1024);
    // return response
    r = std::string(response);
    log << yah::ctime << "[Http] Receive " << r << endl;
    return Response(r);
  error:
    return Response();
  }

  Response::Response(const std::string& text) {
    this->text = text;
    this->code = 0;

    auto begin = this->text.begin() + this->text.find(' ');
    ++begin;
    auto end = this->text.begin() + this->text.find(' ', begin - this->text.begin());
    auto s = this->text.substr(begin - this->text.begin(), end - begin);
    this->code = atoi(s.c_str());
  }

  Response::Response() {
    this->text = "";
    this->code = 600;
  }

  size_t Response::status() const {
    return this->code;
  }

  std::string Response::tcp_text() const {
    return this->text;
  }
}
