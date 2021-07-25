#pragma once

#include <string>
#include <vector>
#include <map>

namespace yah {
  class Json;

  class Value {
    // Json的Value可以是字符串、整数、浮点数、null、布尔值、Json
      enum {STR, INT, FLOAT, NUL, BOOL, JSON} type;
      std::string                             s;
      int                                     i;
      double                                  f;
      bool                                    b;
      Json                                    j;
    public:
      Value(const std::string& s);
      Value(int i);
      Value(double f);
      Value();    // null
      Value(bool b);
      Value(const Json& j);

      Value(const Value& v);

      // 将这个Value转成字符串输出
      // 如果Value是一个字符串，那么将其所有的双引号的前面添加转义字符
      std::string serialize() const;
  };

  class Json {
    // Json可以是一个字典(String -> Value)，也可以是一个数组(Value)
    public:
      enum JsonType { ARRAY, DICT } type;
      typedef enum JsonType JsonType;

      Json(JsonType = ARRAY);
      Json(const Json& j);

      std::vector<Value> vec;
      std::map<Value, Value> items;

      void append(const Value& v);
      void set(const std::string& s, const Value& value);
      Value remove(const std::string& s);
      std::string serialize() const;
  };
}
