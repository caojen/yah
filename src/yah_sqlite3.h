/**
 * helper function for run sqlite3
 * using YAH_DATABASE
 */

#ifndef YAH_SQLITE3_H
#define YAH_SQLITE3_H

#include <sqlite3.h>

// open a database
// if target does not exist, create it
// return 0(SQLITE_OK) if success
// param db: point to the pointer of db
// int yah_sqlite_open(const char* dbpath, sqlite3** db);
#define yah_sqlite_open(dbpath, db) sqlite3_open(dbpath, db)

// close a database
#define yah_sqlite_close(db) sqlite3_close(db)

typedef int (*sqlite3_callback_f)(void* callback, int argc, char** argv, char** colName);

// exec a sql statement in db
// return 0(SQLITE_OK) if success
#define yah_sqlite_exec(db, sql, callback, arg) sqlite3_exec(db, sql, callback, arg, NULL)

// exec a sql without callback
#define yah_sqlite_drop(db, sql) yah_sqlite_exec(db, sql, NULL, NULL)

// exec a sql of 'insert'
// ignore all response
#define yah_sqlite_insert(db, sql) yah_sqlite_drop(db, sql)

// exec a sql of 'delete'
// ignore all response
#define yah_sqlite_delete(db, sql) yah_sqlite_drop(db, sql)

// exec a sql of 'update'
// ignore all response
#define yah_sqlite_update(db, sql) yah_sqlite_drop(db, sql)

// exec a sql of select
// using callback to catch all response
#define yah_sqlite_select(db, sql, callback, arg) yah_sqlite_exec(db, sql, callback, arg)

#include <pthread.h>
#include "yah_airodump.h"
/**
 * all these function will use the global variable sqlite* db
 * extern the variable to all other components
 */

extern sqlite3* db;
extern pthread_mutex_t db_mutex;

// un/lock the database
// should lock the database before insert, delete, update operations
#define yah_db_lock() pthread_mutex_lock(&db_mutex)
#define yah_db_unlock() pthread_mutex_unlock(&db_mutex)

// get the last insert id of db
// return 0 if no insertion did
#define yah_db_last_id() sqlite3_last_insert_rowid(db)

// open the database, if the database does not exists, create it.
// store db into the global pointer 'db'
// return 0(SQLITE_OK) if success, ignore all callbacks
int yah_open_database();

// close the database
// after closing, the pointer db will be set to NULL
// return 0(SQLITE_OK) if success, ignore all callbacks
int yah_close_database();

// insert data into db
// return 0 if should insert, and insert done.
// return 1 if should not insert and insert not done.
// auto generate data->data.id if success
int yah_airodump_data_insert(struct yah_airodump_data* data);
// subfunctions:
int yah_airodump_data_insert_ap(struct yah_airodump_data* data);
int yah_airodump_data_insert_apstation(struct yah_airodump_data* data);
// should insert function return 1 means that should insert
// return 0 means that shouldn't insert
#define YAH_AIRODUMP_DATA_OLD_DATA 2
#define YAH_AIRODUMP_DATA_SHOULD_INSERT 1
#define YAH_AIRODUMP_DATA_SHOULDNOT_INSERT 0
int yah_airodump_data_insert_ap_should_insert(struct yah_airodump_data* data);
int yah_airodump_data_insert_apstation_should_insert(struct yah_airodump_data* data);

// set data in data to 'updated'
// return 0(SQLITE_OK) if success, ignore the callback
int yah_airodump_data_updated(struct yah_airodump_data* data);

// remove all data that will not be used
// using the const YAH_AP_TIME and YAH_APSTATION_TIME
// to determine which rows will be deleted
// return 0(SQLITE_OK) if success, ignore the callback
int yah_airodump_data_de_redundancy();

// fetch all un-updated data from database
// store all rows into param struct yah_airodump_data** data
// the size of data is stored into unsigned* size
// if the data is empty, size will be set to 0, data will be null
// return 0(SQLITE_OK) if success, ignore other callbacks
int yah_airodump_data_fetch_unupdated(struct yah_airodump_data** data, unsigned* size);

// define based sqls using in constructing sqlite3 database
// create ap table sql
#define YAH_DATABASE_AP_SQL "CREATE TABLE if not exists `ap` (id INTEGER PRIMARY KEY AUTOINCREMENT,bssid CHAR(17),comment text,create_time INTEGER not null, is_uploaded int not null default 0);"
#define YAH_DATABASE_APSTATION_SQL "CREATE TABLE if not exists `apstation` (id INTEGER PRIMARY KEY AUTOINCREMENT,bssid CHAR (17),station CHAR (17),comment text, create_time INTEGER NOT NULL,is_uploaded int NOT NULL DEFAULT 0)"

// define sql here
#define YAH_AP_SHOULD_INSERT_SQL "SELECT count(1) AS count FROM ap WHERE bssid = ? AND create_time > ?"
#define YAH_APSTATION_SHOULD_INSERT_SQL "SELECT count(1) AS count FROM apstation WHERE station = ? AND create_time > ?"
#define YAH_AP_UPDATE_SQL "UPDATE ap SET is_uploaded = 1 WHERE id = ?"
#define YAH_APSTATION_UPDATE_SQL "UPDATE apstation SET is_uploaded = 1 WHERE id = ?"
#define YAH_AP_RM_REDUN_SQL "DELETE FROM ap WHERE is_uploaded = 1 AND create_time <= %d"
#define YAH_APSTATION_RM_REDUN_SQL "DELETE FROM apstation WHERE is_uploaded = 1 AND create_time < %d"
#define YAH_AP_INSERT_SQL "INSERT INTO ap(bssid, comment, create_time) VALUES('%q', '%q', %d)"
#define YAH_APSTATION_INSERT_SQL "INSERT INTO apstation(bssid, station, comment, create_time) VALUES('%q', '%q', '%q', %d)"
#define YAH_AP_COUNT_SQL "SELECT count(1) AS count FROM ap WHERE is_uploaded = 0"
#define YAH_APSTATION_COUNT_SQL "SELECT count(1) AS count FROM apstation WHERE is_uploaded = 0"
#define YAH_AP_UNUPDATE_SQL "SELECT id, bssid, comment, create_time FROM ap WHERE is_uploaded = 0"
#define YAH_APSTATION_UNUPDATE_SQL "SELECT id, bssid, station, comment, create_time FROM apstation WHERE is_uploaded = 0"

// db should be opened before calling this function
// init the database using YAH_DATABASE_AP_SQL and YAH_DATABASE_APSTATION_SQL
// return 0(SQLITE_OK) if success. ignore the callback
int yah_airodump_database_init();
#endif
