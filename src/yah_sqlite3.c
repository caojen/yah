#include <stdio.h>
#include <pthread.h>
#include <string.h>

#include "yah_sqlite3.h"
#include "yah_const.h"
#include "yah_log.h"
#include "yah_mem.h"

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
    int test;
    if(data->data.ap.id > 0) {
        test = YAH_AIRODUMP_DATA_OLD_DATA;
    } else {
        test = yah_airodump_data_insert_ap_should_insert(data);
    }
    if(test == YAH_AIRODUMP_DATA_SHOULD_INSERT) {
        char* sql = sqlite3_mprintf(YAH_AP_INSERT_SQL, data->data.ap.bssid, data->data.ap.comment, data->data.ap.create_time);
        yah_db_lock();
        yah_sqlite_drop(db, sql);
        data->data.ap.id = yah_db_last_id();
        yah_db_unlock();
        return 0;
    } else if(test == YAH_AIRODUMP_DATA_OLD_DATA) {
        return 0;
    }

    return 1;
}

int
yah_airodump_data_insert_apstation(struct yah_airodump_data* data) {
    int test;
    if(data->data.apstation.id > 0) {
        test = YAH_AIRODUMP_DATA_OLD_DATA;
    } else {
        test = yah_airodump_data_insert_apstation_should_insert(data);
    }
    if(test == YAH_AIRODUMP_DATA_SHOULD_INSERT) {
        char* sql = sqlite3_mprintf(YAH_APSTATION_INSERT_SQL,
            data->data.apstation.bssid, data->data.apstation.station,
            data->data.apstation.comment, data->data.apstation.create_time);
        yah_db_lock();
        yah_sqlite_drop(db, sql);
        data->data.apstation.id = yah_db_last_id();
        yah_db_unlock();
        return 0;
    } else if(test == YAH_AIRODUMP_DATA_OLD_DATA) {
        return 0;
    }
    return 1;
}

int
yah_airodump_data_insert_ap_should_insert(struct yah_airodump_data* data) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, YAH_AP_SHOULD_INSERT_SQL, -1, &stmt, NULL);
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
    sqlite3_prepare_v2(db, YAH_APSTATION_SHOULD_INSERT_SQL, -1, &stmt, NULL);
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
            sqlite3_prepare_v2(db, YAH_AP_UPDATE_SQL, -1, &stmt, NULL);
            id = data->data.ap.id;
            break;
        case apstation:
            sqlite3_prepare_v2(db, YAH_APSTATION_UPDATE_SQL, -1, &stmt, NULL);
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
    yah_log("de_redundancy: running sql - %s", query);
    yah_sqlite_drop(db, query);

    query = sqlite3_mprintf(YAH_APSTATION_RM_REDUN_SQL, time(NULL) - YAH_APSTATION_TIME);
    yah_log("de_redundancy: running sql - %s", query);
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

int
yah_airodump_data_fetch_unupdated(struct yah_airodump_data*** data, unsigned* size) {
    sqlite3_stmt* count_stmt;
    sqlite3_stmt *ap_stmt, *apstation_stmt;
    int ap_size = 0, apstation_size = 0;
    int res = 0;
    // count ap
    res = sqlite3_prepare_v2(db, YAH_AP_COUNT_SQL, -1, &count_stmt, NULL);
    res = sqlite3_step(count_stmt);
    if(res != SQLITE_ROW) {
        yah_quit("sqlite3 count ap failed");
    }
    ap_size = sqlite3_column_int(count_stmt, 0);
    yah_log("remain ap size = %d", ap_size);
    sqlite3_finalize(count_stmt); count_stmt = NULL;

    // count apstation
    sqlite3_prepare_v2(db, YAH_APSTATION_COUNT_SQL, -1, &count_stmt, NULL);
    if(sqlite3_step(count_stmt) != SQLITE_ROW) {
        yah_quit("sqlite3 count apstation failed");
    }
    apstation_size = sqlite3_column_int(count_stmt, 0);
    yah_log("remain apstation size = %d", apstation_size);
    sqlite3_finalize(count_stmt); count_stmt = NULL;
    *size = ap_size + apstation_size;

    if(*size == 0) {
        // empty, just return
        *data = NULL;
        return 0;
    }
    // have data. save to *data
    *data = (struct yah_airodump_data**) yah_mem_alloc (sizeof(struct yah_airodump_data*) * *size);
    for(unsigned i = 0; i < *size; i++) {
        (*data)[i] = (struct yah_airodump_data*) yah_mem_alloc(sizeof(struct yah_airodump_data));
    }

    sqlite3_prepare_v2(db, YAH_AP_UNUPDATE_SQL, sizeof(YAH_AP_UNUPDATE_SQL), &ap_stmt, NULL);
    int index = 0;
    // id, bssid, comment, create_time
    while(sqlite3_step(ap_stmt) != SQLITE_DONE) {
        unsigned id = sqlite3_column_int(ap_stmt, 0);
        const const char* bssid = sqlite3_column_text(ap_stmt, 1);
        const const char* comment = sqlite3_column_text(ap_stmt, 2);
        time_t create_time = sqlite3_column_int64(ap_stmt, 3);
        (*data)[index]->type = ap;
        strcpy((*data)[index]->specify, bssid);
        strcpy((*data)[index]->data.ap.bssid, bssid);
        strcpy((*data)[index]->data.ap.comment, comment);
        (*data)[index]->data.ap.create_time = create_time;
        (*data)[index]->data.ap.id = id;
        index++;
    }
    sqlite3_finalize(ap_stmt);

    sqlite3_prepare_v2(db, YAH_APSTATION_UNUPDATE_SQL, sizeof(YAH_APSTATION_UNUPDATE_SQL), &apstation_stmt, NULL);
    // id, bssid, station, comment, create_time
    while(sqlite3_step(apstation_stmt) != SQLITE_DONE) {
        unsigned id = sqlite3_column_int(apstation_stmt, 0);
        const char* bssid = sqlite3_column_text(apstation_stmt, 1);
        const char* station = sqlite3_column_text(apstation_stmt, 2);
        const char* comment = sqlite3_column_text(apstation_stmt, 3);
        time_t create_time = sqlite3_column_int64(apstation_stmt, 4);
        (*data)[index]->type = apstation;
        strcpy((*data)[index]->specify, station);
        strcpy((*data)[index]->data.apstation.bssid, bssid);
        strcpy((*data)[index]->data.apstation.station, bssid);
        strcpy((*data)[index]->data.apstation.comment, comment);
        (*data)[index]->data.apstation.create_time = create_time;
        (*data)[index]->data.apstation.id = id;
        index++;
    }

    sqlite3_finalize(apstation_stmt);
    return 0;
}
