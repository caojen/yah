#include <sstream>
#include <iterator>

#include "dict.hpp"

namespace yah {
  Value::Value(const std::string& s) {
    this->type = STR;
    this->s = s;
  }

  Value::Value(int i) {
    this->type = INT;
    this->i = i;
  }

  Value::Value(double f) {
    this->type = FLOAT;
    this->f = f;
  }

  Value::Value() {
    this->type = NUL;
  }

  Value::Value(bool b) {
    this->type = BOOL;
    this->b = b;
  }

  Value::Value(unsigned i) {
    this->type = INT;
    this->i = i;
  }

  Value::Value(const Value& v) {
    this->type = v.type;
    switch(this->type) {
      case STR: {
        this->s = v.s;
        break;
      }
      case INT: {
        this->i = v.i;
        break;
      }
      case FLOAT: {
        this->f = v.f;
        break;
      }
      case NUL: {
        break;
      }
      case BOOL: {
        this->b = v.b;
        break;
      }
    }
  }

  std::string Value::serialize() const {
    std::ostringstream ostr("");

    switch(this->type) {
      case STR: {
        ostr << '"';
        for(auto& ch: this->s) {
          if(ch == '"') {
            ostr << "\\";
          }
          ostr << ch;
        }
        ostr << '"';

        goto output;
      }
      case INT: {
        ostr << this->i;
        goto output;
      }
      case FLOAT: {
        ostr << this->f;
        goto output;
      }
      case NUL: {
        ostr << "null";
        goto output;
      }
      case BOOL: {
        ostr << (this->b ? "true" : "false");
        goto output;
      }
    }

    output:
      return ostr.str();
  }

  bool Value::operator<(const Value& other) const {
    if(this->type < other.type) {
      return true;
    } else if(this->type > other.type) {
      return false;
    }

    switch (this->type) {
      case STR: {
        return this->s < other.s;
      }
      case INT: {
        return this->i < other.i;
      }
      case FLOAT: {
        return this->f < other.f;
      }
      case NUL: {
        return this < &other;
      }
      case BOOL: {
        return this->b < other.b;
      }
    }

    return this < &other;
  }

  Json::Json(JsonType type) {
    this->type = type;
  }

  Json::Json(const Json& j) {
    this->type = j.type;
    switch(this->type) {
      case ARRAY: {
        this->vec = j.vec;
        break;
      }
      case DICT: {
        this->items = j.items;
        break;
      }
    }
  }

  void Json::append(const Value& v) {
    this->vec.push_back(v);
  }

  void Json::set(const std::string& s, const Value& v) {
    this->items[s] = v;
  }

  Value Json::remove(const std::string& s) {
    auto it = this->items.find(s);
    if(it == this->items.end()) {
      return Value();
    } else {
      Value ret = it->second;
      this->items.erase(it);
      return ret;
    }
  }

  std::string Json::serialize() const {

    std::ostringstream ostr("");
    switch (this->type) {
      case ARRAY: {
        ostr << '[';
        for(auto it = this->vec.begin(); it != this->vec.end(); ++it) {
          ostr << it->serialize();
          if(std::next(it) != this->vec.end()) {
            ostr << ',';
          }
        }
        ostr << ']';
        break;
      }

      case DICT: {
        ostr << '{';
        for(auto it = this->items.begin(); it != this->items.end(); ++it) {
          ostr << it->first.serialize();
          ostr << ':';
          ostr << it->second.serialize();
          if(std::next(it) != this->items.end()) {
            ostr << ',';
          }
        }
        ostr << '}';
        break;
      }
    }

    return ostr.str();
  }

  std::string Json::serialize(const std::vector<Json>& vec) {
    std::ostringstream ostr("");
    ostr << "[";
    for(auto it = vec.begin(); it != vec.end(); ++it) {
      ostr << it->serialize();
      if(std::next(it) != vec.end()) {
        ostr << ",";
      }
    }
    ostr << "]";
    return ostr.str();
  }

  std::ostream& operator<<(std::ostream& o, const Json& j) {
    o << j.serialize();
    return o;
  }
}
