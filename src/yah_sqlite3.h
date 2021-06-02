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
#define yah_sqlite_oprn(dbpath, db) sqlite3_open(dbpath, db)

// close a database
#define yah_sqlite_close(db) sqlite3_close(db)

typedef int (*sqlite3_callback_f)(void* callback, int argc, char** argv, char** colName);

// exec a sql statement in db
// return 0(SQLITE_OK) if success
#define yah_sqlite_exec(db, sql, callback, arg) sqlite3_exec(db, sql, callback, arg)

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

#include "yah_airodump.h"
/**
 * all these function will use the global variable sqlite* db
 * extern the variable to all other components
 */

extern sqlite3* db;

// open the database
// store db into the global pointer 'db'
// return 0(SQLITE_OK) if success, ignore all callbacks
int yah_open_database();

// close the database
// after closing, the pointer db will be set to NULL
// return 0(SQLITE_OK) if success, ignore all callbacks
int yah_close_database();

// insert data into db
// return 0(SQLITE_OK) if success, ignore the callback
// auto generate data->data.id if success
int yah_airodump_data_insert(struct yah_airodump_data* data);

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

#endif
