#include <sqlite3.h>
#include <string>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "cache.hpp"
#include "log.hpp"
#include "encode.hpp"
#include "global.hpp"
#include "thread_pool.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string.hpp"

static inline std::string parse_time(time_t t) {
  char r[32] = { 0 };
  struct tm* tm = localtime(&t);
  strftime(r, 32, "%Y-%m-%d %H:%M:%S", tm);
  return std::string(r);
}

class Blue {
public:
  unsigned id;
  std::string address;
  std::string name;
  time_t create_time;

  bool in_db(sqlite3* db) {
    auto address = this->address.c_str();
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT count(1) FROM blue WHERE mac = ? AND create_time > ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, address, this->address.size(), SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, this->create_time - 86400);
    int ret = sqlite3_step(stmt);
    bool is_in = false;
    if (ret == SQLITE_ROW) {
      int count = sqlite3_column_int(stmt, 0);
      if(count > 0) {
        is_in = true;
      }
    } else {
      is_in = true;
    }
    sqlite3_finalize(stmt);

    return is_in;
  }
  void sync(sqlite3* db) {
    char* sql = sqlite3_mprintf("INSERT INTO blue(mac, name, create_time) VALUES('%q', '%q', %d)", this->address.c_str(), this->name.c_str(), this->create_time);
    sqlite3_exec(db, sql, NULL, NULL, NULL);
    this->id = sqlite3_last_insert_rowid(db);
  }
  yah::Json serialize() const {
    auto ret = yah::Json(yah::Json::JsonType::DICT);
    ret.set("id", this->id);
    ret.set("mobile", yah::config.device);
    ret.set("mac_address", this->address);
    ret.set("create_time", parse_time(this->create_time));
    ret.set("etl_time", parse_time(time(NULL)));
    return ret;
  }
};

sqlite3* init_db_connection();
void init_db(sqlite3*);
std::vector<std::unique_ptr<Blue>> fetch_old_data(sqlite3*);

yah::Cache<std::string>* cache;
yah::AutoPool<Blue>* pool;

void init_cache();
void add_old_data_to_queue(std::vector<std::unique_ptr<Blue>>&&);
std::unique_ptr<Blue> line2blue(char* line);

int main(int argc, char**argv) {
  if(argc != 2) {
    std::cout << "Usage: " << argv[0] << " {log_file}" << std::endl;
    exit(1);
  }
  // 初始化所有配置
  yah::config = yah::Config(argv[1]);
  // 初始化数据库连接
  auto db = init_db_connection();
  // 初始化数据库
  init_db(db);
  // 删除旧的数据，然后获取未上传的数据
  auto old_data = fetch_old_data(db);
  // 初始化缓冲池
  init_cache();
  // 初始化pool
  // 池的内容，每过5+(10)秒就发送一次数据，每次发送的数据量不超过100
  pool = new yah::AutoPool<Blue>(1, 100, [](std::vector<std::unique_ptr<Blue>>& data) {
    std::cout << "Get " << data.size() << " number of Blue" << std::endl;
    auto _host = yah::config.remote_address;
    auto _port = yah::config.remote_port;
    auto _path = "/pidc/m2btdevice/uploadM2Data.action";

    std::vector<yah::Json> arr;
    for(auto& item: data) {
      arr.push_back(item->serialize());
    }
    std::string main_data = yah::Json::serialize(arr);
    std::cout << "[main data] " << main_data << std::endl;
    yah::Encode encoder(main_data);
    std::string data_encoded = encoder.encode();
    std::cout << "[data encoded] " << data_encoded << std::endl;
    yah::Json json; json.set("data", data_encoded);

  }, 10);
  // 将未上传的数据加入到pool中
  add_old_data_to_queue(std::move(old_data));

  // 创建bluetoothctl进程，捕获输出，并格式化。
  // 如果格式化后的数据不在缓冲池中，那么判断是否在数据库中。如果不在数据库中，那么将这个数据加入到缓冲池和数据库中，再加入到池中
  while(true) {
    std::cout << "[Running]" << std::endl;
    auto fp = popen("bluetoothctl --timeout 10 scan on", "r");
    char *buf;
    size_t len = 0;

    while(getline(&buf, &len, fp) != -1) {
      auto blue = line2blue(buf);
      if(blue != nullptr) {
        if(cache->insert(blue->address)) {
          if(!blue->in_db(db)) {
            std::cout << "[New-Store] " << blue->address << " " << blue->name << std::endl;
            blue->sync(db);
            pool->push(std::move(blue));
          }
        }
      }
    }

    pclose(fp);
  }
}

sqlite3* init_db_connection() {
  sqlite3* db = NULL;
  sqlite3_open(yah::config.db.c_str(), &db);
  return db;
}

void init_db(sqlite3* db) {
  const char* blue = "CREATE TABLE if not exists `blue` (id INTEGER PRIMARY KEY AUTOINCREMENT, mac CHAR(17), name VARCHAR(1024), create_time INTEGER not null, is_uploaded int not null default 0);";
  sqlite3_exec(db, blue, NULL, NULL, NULL);

  const char* remove_blue = "DELETE FROM blue WHERE is_uploaded = 1;";
  sqlite3_exec(db, remove_blue, NULL, NULL, NULL);
}

std::vector<std::unique_ptr<Blue>> fetch_old_data(sqlite3* db) {
  std::vector<std::unique_ptr<Blue>> ret;

  sqlite3_stmt* stmt = nullptr;
  sqlite3_prepare_v2(db, "SELECT id, mac, name, create_time FROM `blue` WHERE is_uploaded = 0;", -1, &stmt, NULL);
  while(sqlite3_step(stmt) != SQLITE_DONE) {
    auto ptr = new Blue();
    ptr->id  = sqlite3_column_int(stmt, 0);
    ptr->address = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
    ptr->name = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
    ptr->create_time = sqlite3_column_int64(stmt, 3);

    std::cout << "[OldData] " << ptr->id << " " << ptr->address << " " << ptr->name << std::endl;
    ret.push_back(std::unique_ptr<Blue>(ptr));
  }

  sqlite3_finalize(stmt);

  return ret;
}

void init_cache() {
  cache = new yah::Cache<std::string>(10000, 86400);
}

void add_old_data_to_queue(std::vector<std::unique_ptr<Blue>>&& data) {
  for(auto& item: data) {
    pool->push(std::move(item));
  }
}

std::unique_ptr<Blue> line2blue(char* line) {
  std::string s = boost::trim_copy(std::string(line));
  if(s.size() > 0) {
    std::vector<std::string> splits;
    boost::split(splits, s, boost::is_any_of(" "));
    if(splits.size() >= 4) {
      if(splits[1] == "Device") {
        std::string mac = splits[2];
        std::string name = "";
        for(int i = 3; i < splits.size(); ++i) {
          name += splits[i];
        }
        auto ret = new Blue();
        ret->address = mac;
        ret->name = name;
        ret->create_time = time(NULL);
        return std::unique_ptr<Blue>(ret);
      }
    }
  }
  return nullptr;
}