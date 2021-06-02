#include <stdio.h>

#include "yah_sqlite3.h"

int main() {
    printf("hello world!\n");
    yah_open_database();
    yah_close_database();

    int res = 0;

    struct yah_airodump_data* data;
    int size;
    res = yah_airodump_data_fetch_unupdated(&data, &size);
    printf("done, fetch size = %d, res = %d\n", size, res);
    return 0;
}
