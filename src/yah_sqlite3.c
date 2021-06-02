#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "yah_sqlite3.h"
#include "yah_const.h"
#include "yah_log.h"

sqlite3* db = NULL;
pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

int
yah_open_database() {
    const char* path = YAH_DATABASE;
    int rc;

    // if db is opened, close it
    if(db != NULL) {
        yah_close_database(db);
    }

    // open the database, storing into db
    rc = yah_sqlite_open(path, &db);
    if(rc != SQLITE_OK) {
        db = NULL;
    } else {
        // init the database
        yah_airodump_database_init();
    }
    return rc;
}

int
yah_close_database() {
    int rc = SQLITE_OK;
    if(db != NULL) {
        yah_sqlite_close(db);
    }
    return rc;
}

int
yah_airodump_data_insert(struct yah_airodump_data* data) {
    int r = 1;
    switch (data->type) {
        case ap:
            r &= yah_airodump_data_insert_ap(data);
            break;
        case apstation:
            r &= yah_airodump_data_insert_apstation(data);
            break;
        default:
            unreachable();
    }

    return r;
}

int
yah_airodump_data_insert_ap(struct yah_airodump_data* data) {
    if(yah_airodump_data_insert_ap_should_insert(data) == YAH_AIRODUMP_DATA_SHOULD_INSERT) {
        char* sql = sqlite3_mprintf(YAH_AP_INSERT_SQL, data->data.ap.bssid, data->data.ap.comment, data->data.ap.create_time);
        yah_db_lock();
        yah_sqlite_drop(db, sql);
        data->data.ap.id = yah_db_last_id();
        yah_db_unlock();
        return 0;
    }

    return 1;
}

int
yah_airodump_data_insert_apstation(struct yah_airodump_data* data) {
    if(yah_airodump_data_insert_apstation_should_insert(data) == YAH_AIRODUMP_DATA_SHOULD_INSERT) {
        char* sql = sqlite3_mprintf(YAH_APSTATION_INSERT_SQL,
            data->data.apstation.bssid, data->data.apstation.station,
            data->data.apstation.comment, data->data.apstation.create_time);
        yah_db_lock();
        yah_sqlite_drop(db, sql);
        data->data.apstation.id = yah_db_last_id();
        yah_db_unlock();
        return 0;
    }
    return 1;
}

int
yah_airodump_data_insert_ap_should_insert(struct yah_airodump_data* data) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, YAH_AP_SHOULD_INSERT_SQL, sizeof(YAH_AP_SHOULD_INSERT_SQL), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, data->specify, strlen(data->specify), SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, data->data.ap.create_time - YAH_AP_TIME);
    int ret = sqlite3_step(stmt);
    if(ret == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if(count > 0) {
            return YAH_AIRODUMP_DATA_SHOULDNOT_INSERT;
        }
    } else if(ret == SQLITE_DONE) {
        yah_warn("sqlite3: ap should insert, return SQLITE_DONE, but not SQLITE_ROW");
    }
    sqlite3_finalize(stmt);
    return YAH_AIRODUMP_DATA_SHOULD_INSERT;
}

int
yah_airodump_data_insert_apstation_should_insert(struct yah_airodump_data* data) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, YAH_APSTATION_SHOULD_INSERT_SQL, sizeof(YAH_APSTATION_SHOULD_INSERT_SQL), &stmt, NULL);
    sqlite3_bind_text(stmt, 1, data->specify, strlen(data->specify), SQLITE_STATIC);
    sqlite3_bind_int64(stmt, 2, data->data.apstation.create_time - YAH_APSTATION_TIME);
    int ret = sqlite3_step(stmt);
    if(ret == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if(count > 0) {
            return YAH_AIRODUMP_DATA_SHOULDNOT_INSERT;
        }
    } else if(ret == SQLITE_DONE) {
        yah_warn("sqlite3: apstation should insert, return SQLITE_DONE, but not SQLITE_ROW");
    }
    sqlite3_finalize(stmt);
    return YAH_AIRODUMP_DATA_SHOULD_INSERT;
}

int
yah_airodump_data_updated(struct yah_airodump_data* data) {
    sqlite3_stmt* stmt;
    unsigned id = 0;
    yah_db_lock();
    switch (data->type) {
        case ap:
            sqlite3_prepare_v2(db, YAH_AP_UPDATE_SQL, sizeof(YAH_AP_UPDATE_SQL), &stmt, NULL);
            id = data->data.ap.id;
            break;
        case apstation:
            sqlite3_prepare_v2(db, YAH_APSTATION_UPDATE_SQL, sizeof(YAH_APSTATION_UPDATE_SQL), &stmt, NULL);
            id = data->data.apstation.id;
            break;
        default:
            unreachable();
    }
    sqlite3_bind_int(stmt, 1, id);
    if(sqlite3_step(stmt) != SQLITE_DONE) {
        yah_warn("sqlite3: update does not return SQLITE_DONE");
    }
    sqlite3_finalize(stmt);
    yah_db_unlock();
    return 0;
}

int
yah_airodump_data_de_redundancy() {
    char* query;
    query = sqlite3_mprintf(YAH_AP_RM_REDUN_SQL, time(NULL) - YAH_AP_TIME);
    yah_sqlite_drop(db, query);

    query = sqlite3_mprintf(YAH_APSTATION_RM_REDUN_SQL, time(NULL) - YAH_APSTATION_TIME);
    yah_sqlite_drop(db, query);
    return 0;
}


int
yah_airodump_database_init() {
    int r = 0;
    r |= yah_sqlite_drop(db, YAH_DATABASE_AP_SQL);
    r |= yah_sqlite_drop(db, YAH_DATABASE_APSTATION_SQL);
    return r;
}
