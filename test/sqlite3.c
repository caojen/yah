#include <stdio.h>
#include <string.h>
#include <time.h>

#include "yah_sqlite3.h"

int main() {
    printf("hello world!\n");
    yah_open_database();
    // yah_close_database();

    int res = 0;

    struct yah_airodump_data* data;
    int size;
    printf("in\n");
    yah_airodump_data_de_redundancy();
    res = yah_airodump_data_fetch_unupdated(&data, &size);
    printf("done, fetch size = %d, res = %d\n", size, res);

    printf("time = %lu\n", time(NULL));
    struct yah_airodump_data d;
    memset(&d, 0, sizeof(d));
    d.type = ap;
    strcpy(d.specify, "1234568989");
    d.data.ap.create_time = 1622630073;
    strcpy(d.data.ap.comment, "comment  skk   2123  1 - 1 0 1 -1");
    strcpy(d.data.ap.bssid, d.specify);
    res = yah_airodump_data_insert_ap_should_insert(&d);
    printf("ap should insert return res = %d\n", res);

    res = yah_airodump_data_insert_ap(&d);
    printf("insert done, ap return res = %d, id = %d\n", res, d.data.ap.id);
    yah_close_database();
    return 0;
}
