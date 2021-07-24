#include "dict.hpp"

using namespace yah;

Dict::Dict(const std::string& key, const int& v) {
  this->key = key;
  this->v_int = v;
  this->jsonType = Int;
}

Dict::Dict(const std::string& key, const double& v) {
  this->key = key;
  this->v_float = v;
  this->jsonType = Float;
}

Dict::Dict(const std::string& key, const bool& v) {
  this->key = key;
  this->v_bool = v;
  this->jsonType = Bool;
}

Dict::Dict(const std::string& key, const enum DictType& v) throw(std::bad_typeid) {
  if (v == Null) {
    this->key = key;
    this->v_null = true;
    this->jsonType = Null;
  } else {
    throw std::bad_typeid();
  }
}

Dict::Dict(const std::string& key, const std::string& v) {
  this->key = key;
  this->v_string = v;
  this->jsonType = String;
}

enum Dict::DictType Dict::type() const {
  return this->jsonType;
}

void Dict::set_key(const std::string& key) {
  this->key = key;
}

void Dict::set_v(const int& v) {
  this->v_int = v;
  this->jsonType = Int;
}

void Dict::set_v(const double& v) {
  this->v_float = v;
  this->jsonType = Float;
}

void Dict::set_v(const bool& v) {
  this->v_bool = v;
  this->jsonType = Bool;
}

void Dict::set_v(const enum DictType& v) {
  if(v == Null) {
    this->v_null = true;
    this->jsonType = Null;
  } else {
    throw std::bad_typeid();
  }
}

void Dict::set_v(const std::string& v) {
  this->v_string = v;
  this->jsonType = String;
}

std::string Dict::serialize() const {
  std::ostringstream ostr;
  // Bug：key中不能够有引号字符
  ostr << "{\"" << this->key << "\":";
  switch (this->jsonType) {
    case Int: {
      ostr << this->v_int;
      break;
    }
    case Float: {
      ostr << this->v_float;
      break;
    }
    case Bool: {
      ostr << (this->v_bool ? "true" : "false");
      break;
    }
    case Null: {
      ostr << "null";
      break;
    }
    case String: {
      ostr << "\"" << this->v_string << "\"";
      break;
    }
  }
  ostr << "}";
  return ostr.str();
}

std::string Dict::serialize(const std::vector<Dict>& dicts) {
  std::ostringstream str;
  str << "[";
  unsigned size = dicts.size();
  for(unsigned i = 0; i < size; i++) {
    str << dicts[i].serialize();
    if (i != size - 1) {
      str << ",";
    }
  }

  str << "]";

  return str.str();
}

std::ostream& yah::operator<<(std::ostream& o, const yah::Dict& dict) {
  o << dict.serialize();
  return o;
}
