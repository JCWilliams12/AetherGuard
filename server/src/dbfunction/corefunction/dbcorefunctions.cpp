#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include "dbcorefunctions.hpp"

extern "C" {
    #include "sqlite3.h"
}

const char* DB_NAME = "server/database/app.db";

sqlite3 *db = nullptr;
// -=- 1. CREATE TABLE -=-
// Creates the table only if it doesn't exist yet.
void createTable() {
    //sqlite3 *db;  This was opening the database every time we called createTable, which is not what we want. We should open it once and keep it open.
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(DB_NAME, &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Updated Schema: freq, time, location, rawT, summary, channelName
    // Composite Primary Key: freq, time, location
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS RadioLogs (" \
        "freq REAL, " \
        "time INTEGER, " \
        "location TEXT, " \
        "rawT TEXT, " \
        "summary TEXT, " \
        "channelName TEXT, " \
        "PRIMARY KEY (freq, time, location));";

    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table 'RadioLogs' created or verified successfully with new schema.\n");
    }

    //sqlite3_close(db);  Can't sort cause its a closed connection.
}

// -=- 2. INSERT ROW -=-
// Uses "INSERT OR REPLACE" to update existing logs that match the primary key
void insertLog(double freq, long long time, std::string location, std::string rawT, std::string summary, std::string channelName) {
    if (!db) {
        std::cerr << "Error: Database not initialized!" << std::endl;
        return; // or return; for void functions
}
    sqlite3_stmt *stmt;
    int rc;


    const char *sql = "INSERT OR REPLACE INTO RadioLogs (freq, time, location, rawT, summary, channelName) VALUES (?, ?, ?, ?, ?, ?);";

    // Prepare the statement
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return;
    }

    // Bind values to the ? placeholders (Indices start at 1)
    sqlite3_bind_double(stmt, 1, freq);
    sqlite3_bind_int64(stmt, 2, time);
    sqlite3_bind_text(stmt, 3, location.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, rawT.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, summary.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, channelName.c_str(), -1, SQLITE_STATIC);

    // Execute
    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
    } else {
        std::cout << "Log inserted successfully: " << channelName << " (" << freq << "MHz) at " << time << std::endl;
    }

    // Clean up
    sqlite3_finalize(stmt);
    // Do NOT close the database here, as we want to keep it open for the app's lifetime.
}

// -=- 3. QUERY ALL -=-
std::vector<RadioLog> getAllLogs() {
    std::cout << "Debug: db pointer is " << db << std::endl;
    if (!db) {
        std::cerr << "Error: Database not initialized!" << std::endl;
        return {}; // Return empty vector if database is not initialized
    }
    std::vector<RadioLog> logs;
    int rc;
    sqlite3_stmt *stmt;


    const char *sql = "SELECT freq, time, location, rawT, summary, channelName FROM RadioLogs ORDER BY time DESC;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "SQL error: " << sqlite3_errmsg(db) << std::endl;
       
        return logs;
    }

    std::cout << "\n--- Terminal Debug: Fetching Logs ---" << std::endl;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        RadioLog log;
        log.freq        = sqlite3_column_double(stmt, 0);
        log.time        = sqlite3_column_int64(stmt, 1);

        const unsigned char* loc = sqlite3_column_text(stmt, 2);
        log.location = loc ? reinterpret_cast<const char*>(loc) : "";

        const unsigned char* rawText = sqlite3_column_text(stmt, 3);
        log.rawT = rawText ? reinterpret_cast<const char*>(rawText) : "";

        const unsigned char* sum = sqlite3_column_text(stmt, 4);
        log.summary = sum ? reinterpret_cast<const char*>(sum) : "";

        const unsigned char* cName = sqlite3_column_text(stmt, 5);
        log.channelName = cName ? reinterpret_cast<const char*>(cName) : "";

        std::cout << "Found: " << log.channelName << " | " << log.freq << " MHz" << std::endl;

        logs.push_back(log);
    }
    
    std::cout << "Total logs sent to frontend: " << logs.size() << "\n" << std::endl;

    sqlite3_finalize(stmt);
    
    return logs;
}

// -=- 4. REMOVE ROW -=-
// Deletes a specific log based on its unique Composite Key (Freq + Time + Location)
int removeLog(double freq, long long time, std::string location) {
    if (!db) {
        std::cerr << "Error: Database not initialized!" << std::endl;
        return 0; // Return 0 to indicate failure
    }
    sqlite3_stmt *stmt;
    int rc;

    // Now matching all three parts of the composite key
    const char *sql = "DELETE FROM RadioLogs WHERE ROUND(freq, 3) = ROUND(?, 3) AND time = ? AND location = ?;";
        
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_double(stmt, 1, freq);
    sqlite3_bind_int64(stmt, 2, time);
    sqlite3_bind_text(stmt, 3, location.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Delete failed: %s\n", sqlite3_errmsg(db));
    } else {
        if (sqlite3_changes(db) > 0) {
            std::cout << "✅ SUCCESS: Deleted log: " << freq << "MHz at " << time << " in " << location << std::endl;
        } else {
            std::cout << "❌ FAILED: No exact match found to delete." << std::endl;
        }
    }

    sqlite3_finalize(stmt);
    return 1; 
}

// -=- 5. OPEN DATABASE CHECK -=-
//Test Function, not needed.
/*void openDatabase(){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(DB_NAME, &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }
    sqlite3_close(db);
}*/