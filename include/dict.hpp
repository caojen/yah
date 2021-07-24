#pragma once

#include <vector>
#include <string>
#include <exception>
#include <sstream>
#include <iostream>

namespace yah {

  class Dict {
    private:
      std::string key;
      
      // Dict的值可能是整数，浮点数，布尔值，null，字符串
      enum DictType { Int, Float, Bool, Null, String } jsonType;

      int v_int;
      double v_float;
      bool v_bool;
      bool v_null;
      std::string v_string;

    public:
      Dict(const std::string& key, const int& v);
      Dict(const std::string& key, const double& v);
      Dict(const std::string& key, const bool& v);
      Dict(const std::string& key, const enum DictType& v) throw (std::bad_typeid);
      Dict(const std::string& key, const std::string& v);

      // 返回这个json的值类型
      enum DictType type() const;
      // 重设这个json的key
      void set_key(const std::string& key);
      // 重设这个json的v
      void set_v(const int& v);
      void set_v(const double& v);
      void set_v(const bool& v);
      void set_v(const enum DictType& v);
      void set_v(const std::string& v);
      // 将这个json序列化
      std::string serialize() const;
      // 将多个json序列化
      static std::string serialize(const std::vector<Dict>& dicts);
      friend std::ostream& operator<<(std::ostream& o, const Dict & dict);
  };
}
