#include "db.hpp"

using namespace yah;

DB::DB() {}

DB::DB(std::string path) {
  this->path = path;
  pthread_mutex_init(&this->mutex, NULL);

  sqlite3_open(this->path.c_str(), &this->db);
}

DB::~DB() {
  sqlite3_close(db);
}
