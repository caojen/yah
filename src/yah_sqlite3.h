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

typedef int (*callback_f)(void* callback, int argc, char** argv, char** colName);

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

#endif
