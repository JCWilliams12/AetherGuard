#include <iostream>
#include <string>
#include <stdio.h>
extern "C" {
    #include "sqlite3.h"
}

void openDatabase(){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_close(db);
}

int main(){
    openDatabase();
    
}