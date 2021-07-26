#include <pthread.h>

#include "db.hpp"
#include "global.hpp"

using namespace yah;

DB::DB() {}

DB::DB(std::string path) {
  this->path = path;
  pthread_mutex_init(&this->mutex, NULL);
  this->db = NULL;
}

DB::~DB() {
  sqlite3_close(db);
}

void DB::lock() {
  pthread_mutex_lock(&this->mutex);
}

void DB::unlock() {
  pthread_mutex_unlock(&this->mutex);
}
