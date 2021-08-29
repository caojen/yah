#pragma once

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

#include "root_certificates.hpp"

namespace ot {
  typedef enum request_type { get, post } request_type;

  typedef enum request_protocol { http, https } request_protocol;

  template <request_protocol protocol, request_type type> class Request {
  public:
    Request();

    /**
     * 设置host
     * @param _host 目的主机地址，例如: www.google.com 不应该包括前导的http://,
     * 或https://
     */
    void set_host(const std::string &_host);

    /**
     * 设置port，如果不设置，那么默认是http = 80. https = 443
     * @param port
     */
    void set_port(unsigned _port);

    /**
     * 设置目标内容，例如 /search?key=1
     * @param _target
     */
    void set_target(const std::string &_target);

    /**
     * 设置请求体内容
     * @param _body
     */
    void set_body(const std::string &_body);

    /**
     * 设置头部内容
     * @param _field key
     * @param _value value
     */
    void set_field(boost::beast::http::field _field, const std::string &_value);

    /**
     * 发送请求
     * 请求发送后，对应的返回头和返回体将会被设置
     */
    void send();

    /**
     * 获取返回的头
     * @param _key
     * @return
     */
    std::string get_response_header(const std::string &_key) const;

    const std::map<std::string, std::string> &get_response_headers() const;

    /**
     * 获取返回的body
     * @return
     */
    std::string get_response_body() const;

  private:
    std::string host;
    unsigned port;
    std::string target;
    std::string body;
    std::map<boost::beast::http::field, std::string> request_headers;

    std::map<std::string, std::string> response_headers;
    std::string response_body;
  };

  template <request_protocol protocol, request_type type>
  Request<protocol, type>::Request() {
    if (protocol == request_protocol::http) {
      port = 80;
    } else if (protocol == request_protocol::https) {
      port = 443;
    }

    // 设置通用的请求头
    this->request_headers[boost::beast::http::field::user_agent] =
        "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) "
        "AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Mobile "
        "Safari/537.36";
    this->request_headers[boost::beast::http::field::connection] = "Close";
    this->request_headers[boost::beast::http::field::accept] = "*/*";
  }

  template <request_protocol protocol, request_type type>
  void Request<protocol, type>::set_host(const std::string &_host) {
    this->host = _host;
    this->request_headers[boost::beast::http::field::host] = _host;
  }

  template <request_protocol protocol, request_type type>
  void Request<protocol, type>::set_port(unsigned int _port) {
    this->port = _port;
  }

  template <request_protocol protocol, request_type type>
  void Request<protocol, type>::set_target(const std::string &_target) {
    this->target = _target;
  }

  template <request_protocol protocol, request_type type>
  void Request<protocol, type>::set_body(const std::string &_body) {
    this->body = _body;
  }

  template <request_protocol protocol, request_type type>
  void Request<protocol, type>::set_field(boost::beast::http::field _field,
                                          const std::string &_value) {
    this->request_headers[_field] = _value;
  }

  template <request_protocol protocol, request_type type>
  void Request<protocol, type>::send() {
    try {
      if (protocol == request_protocol::http) {
        namespace beast = boost::beast; // from <boost/beast.hpp>
        namespace http = beast::http;   // from <boost/beast/http.hpp>
        namespace net = boost::asio;    // from <boost/asio.hpp>
        using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

        net::io_context ioc;
        tcp::resolver resolver(ioc);
        beast::tcp_stream stream(ioc);
        auto const results =
            resolver.resolve(this->host, std::to_string(this->port));
        stream.connect(results);

        auto http_type =
            type == request_type::get ? http::verb::get : http::verb::post;
        http::request<http::string_body> req{http_type, this->target, 11};
        for (auto &header : this->request_headers) {
          req.set(header.first, header.second);
        }
        if (!this->body.empty()) {
          req.body() = this->body;
          req.prepare_payload();
        }

        http::write(stream, req);
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(stream, buffer, res);

        for (auto &h : res.base()) {
          this->response_headers[h.name_string().to_string()] =
              h.value().to_string();
        }
        std::ostringstream ostr;
        ostr << res.body();
        this->response_body = ostr.str();

        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);
        if (ec && ec != beast::errc::not_connected) {
          throw beast::system_error{ec};
        }
      } else if (protocol == request_protocol::https) {
        //        namespace beast = boost::beast; // from <boost/beast.hpp>
        //        namespace http = beast::http;   // from <boost/beast/http.hpp>
        //        namespace net = boost::asio;    // from <boost/asio.hpp>
        //        namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
        //        using tcp = net::ip::tcp;       // from
        //        <boost/asio/ip/tcp.hpp>
        //
        //        net::io_context ioc;
        //        ssl::context ctx(ssl::context::tlsv12_client);
        //        load_root_certificates(ctx);
        //        ctx.set_verify_mode(ssl::verify_peer);
        //
        //        tcp::resolver resolver(ioc);
        //        ssl::stream<beast::tcp_stream> stream(ioc, ctx);
        //
        //        if (!SSL_set_tlsext_host_name(stream.native_handle(),
        //                                      this->host.c_str())) {
        //          beast::error_code ec{static_cast<int>(ERR_get_error()),
        //                               net::error::get_ssl_category()};
        //          throw beast::system_error{ec};
        //        }
        //
        //        auto const results =
        //            resolver.resolve(this->host, std::to_string(this->port));
        //        beast::get_lowest_layer(stream).connect(results);
        //
        //        stream.handshake(ssl::stream_base::client);
        //
        //        auto http_type =
        //            type == request_type::get ? http::verb::get :
        //            http::verb::post;
        //        http::request<http::string_body> req{http_type, this->target,
        //        11}; for (auto &header : this->request_headers) {
        //          req.set(header.first, header.second);
        //        }
        //
        //        if (!this->body.empty()) {
        //          req.body() = this->body;
        //          req.prepare_payload();
        //        }
        //
        //        http::write(stream, req);
        //        beast::flat_buffer buffer;
        //        http::response<http::string_body> res;
        //        http::read(stream, buffer, res);
        //
        //        for (auto &h : res.base()) {
        //          this->response_headers[h.name_string().to_string()] =
        //              h.value().to_string();
        //        }
        //        std::ostringstream ostr;
        //        ostr << res.body();
        //        this->response_body = ostr.str();
        //
        //        beast::error_code ec;
        //        stream.shutdown(ec);
        //        if (ec == net::error::eof ||
        //            ec == boost::asio::ssl::error::stream_truncated) {
        //          ec = {};
        //        }
        //        if (ec) {
        //          throw beast::system_error(ec);
        //        }
        //      }
      }
    } catch (const std::exception &e) {
      std::cerr << "Request to " << this->host << ":" << this->port << " error: " << e.what();
    }
  }

  template <request_protocol protocol, request_type type>
  std::string
  Request<protocol, type>::get_response_header(const std::string &_key) const {
    return this->response_headers.find(_key)->second;
  }

  template <request_protocol protocol, request_type type>
  std::string Request<protocol, type>::get_response_body() const {
    return this->response_body;
  }

  template <request_protocol protocol, request_type type>
  const std::map<std::string, std::string> &
  Request<protocol, type>::get_response_headers() const {
    return this->response_headers;
  }
}
