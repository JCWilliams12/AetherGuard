#include <iostream>
#include <string>
#include <stdio.h>
#include "dbcorefunctions.hpp"
extern "C" {
    #include "sqlite3.h"
}

const char* DB_NAME = "server/database/app.db";

// -=- CREATE TABLE -=-
// Creates the table only if it doesn't exist yet.
void createTable() {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(DB_NAME, &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = 
        "CREATE TABLE IF NOT EXISTS RadioLogs (" \
        "radiofrequency REAL, " \
        "time INTEGER, " \
        "location TEXT, " \
        "text TEXT, " \
        "textSummary TEXT, " \
        "PRIMARY KEY (radiofrequency, time));";

    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table 'RadioLogs' created or verified successfully\n");
    }

    sqlite3_close(db);
}

// -=- 2. INSERT ROW -=-
// Uses "INSERT OR REPLACE" so if you get a duplicate time/freq, it updates the old one.
void insertLog(double freq, long long time, std::string location, std::string text, std::string summary) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_open(DB_NAME, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = "INSERT OR REPLACE INTO RadioLogs (radiofrequency, time, location, text, textSummary) VALUES (?, ?, ?, ?, ?);";

    // Prepare the statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    // Bind values to the ? placeholders (Indices start at 1)
    sqlite3_bind_double(stmt, 1, freq);
    sqlite3_bind_int64(stmt, 2, time);
    sqlite3_bind_text(stmt, 3, location.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, text.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, summary.c_str(), -1, SQLITE_STATIC);

    // Execute
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    } else {
        std::cout << "Log inserted successfully: " << freq << "MHz at " << time << std::endl;
    }

    // Clean up
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

// -=- 3. QUERY ALL (Helper) -=-
// This function is called once for every row found in the database.
// static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
//     for (int i = 0; i < argc; i++) {
//         printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//     }
//     printf("\n");
//     return 0;
// }

// // -=- QUERY ALL FUNCTION -=-
// void getAllLogs() {
//     sqlite3 *db;
//     char *zErrMsg = 0;
//     int rc;

//     rc = sqlite3_open(DB_NAME, &db);
//     if (rc) {
//         fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//         return;
//     }

//     const char *sql = "SELECT * FROM RadioLogs ORDER BY time DESC;";

//     rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "SQL error: %s\n", zErrMsg);
//         sqlite3_free(zErrMsg);
//     }

//     sqlite3_close(db);
// }

std::vector<RadioLog> getAllLogs() {
    std::vector<RadioLog> logs;
    sqlite3 *db;
    sqlite3_stmt *stmt;

    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) {
        std::cerr << "Can't open database: "  << sqlite3_errmsg(db) << std::endl;
        return logs;
    }

    const char *sql = "SELECT radiofrequency, time, location, text textSummary FROM RadioLogs ORDER BY time DESC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "SQL error: " <<sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return logs;
    }

    std::cout << "\n--- Terminal Deug: Fetching Logs ---" <<std::endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        RadioLog log;
        log.frequency = sqlite3_column_double(stmt, 0);
        log.time      = sqlite3_column_int64(stmt, 1);

        const unsigned char* loc = sqlite3_column_text(stmt, 2);
        log.location = loc ? reinterpret_cast<const char*>(loc) : "";

        const unsigned char* text = sqlite3_column_text(stmt, 3);
        log.text = text ? reinterpret_cast<const char*>(text) : "";

        const unsigned char* sum = sqlite3_column_text(stmt, 4);
        log.summary = sum ? reinterpret_cast<const char*>(sum) : "";

        // Still printing to terminal for you!
        std::cout << "Found: " << log.frequency << " MHz | Summary: " << log.summary << std::endl;

        logs.push_back(log);
    }
    std::cout << "Total logs sent to frontend: " << logs.size() << "\n" << std::endl;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return logs;
}



// -=- 4. REMOVE ROW -=-
// Deletes a specific log based on its unique Composite Key (Freq + Time)
void removeLog(double freq, long long time) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc;

    rc = sqlite3_open(DB_NAME, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    const char *sql = "DELETE FROM RadioLogs WHERE radiofrequency = ? AND time = ?;";

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_double(stmt, 1, freq);
    sqlite3_bind_int64(stmt, 2, time);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Delete failed: %s\n", sqlite3_errmsg(db));
    } else {
        if (sqlite3_changes(db) > 0) {
            std::cout << "Deleted log: " << freq << "MHz at " << time << std::endl;
        } else {
            std::cout << "No log found to delete with those keys." << std::endl;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void openDatabase(){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_close(db);
}