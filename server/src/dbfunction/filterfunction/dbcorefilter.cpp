#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <ctime>
#include <iomanip> //used in get_time 
#include <sstream> // needed for stringstream 
#include "dbcorefilter.hpp"
#include "../corefunction/dbcorefunctions.hpp"

extern "C" {
    #include "sqlite3.h"
}
//keys are freq and time
// Helper to format output in a box for later in the database page
static std::string formatRowAsString(sqlite3_stmt* stmt) {
    const char* freq = (const char*)sqlite3_column_text(stmt, 0);
    const char* time = (const char*)sqlite3_column_text(stmt, 1);
    const char* loc  = (const char*)sqlite3_column_text(stmt, 2);
    const char* txt  = (const char*)sqlite3_column_text(stmt, 3);
    const char* sum  = (const char*)sqlite3_column_text(stmt, 4);

    std::stringstream box;
    box << "| FREQUENCY : " << std::left << std::setw(27) << (freq ? freq : "N/A") << " |\n";
    box << "| TIME      : " << std::left << std::setw(27) << (time ? time : "N/A") << " |\n";
    box << "| LOCATION  : " << std::left << std::setw(27) << (loc  ? loc  : "N/A") << " |\n";
    box << "| SUMMARY   : " << std::left << std::setw(27) << (sum  ? sum  : "N/A") << " |\n";
    box << "| TEXT      : " << std::left << std::setw(27) << (txt  ? txt  : "N/A") << " |\n";
    
    return box.str();
}

// Filter by Frequency
std::string filterByFrequency(double freq) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string results = "";

    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return "Error: DB Connection.";

    const char *sql = "SELECT * FROM RadioLogs WHERE radiofrequency = ? ORDER BY time DESC;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, freq);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results += formatRowAsString(stmt);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return results.empty() ? "No records found for that frequency." : results;
}

// Filter by Location
std::string filterByLocation(std::string loc) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string results = "";

    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return "Error: DB Connection.";

    const char *sql = "SELECT * FROM RadioLogs WHERE location LIKE ? ORDER BY time DESC;";  

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        std::string query = "%" + loc + "%";
        sqlite3_bind_text(stmt, 1, query.c_str(), -1, SQLITE_TRANSIENT);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            results += formatRowAsString(stmt);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return results.empty() ? "No records found for that location." : results;
}

// Filters logs within a 60-second window of the provided Unix timestamp
std::string filterByTime(long long unixTime) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string allResults = "";

    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return "Error: Could not open database.";

    // everything from (time) to (time + 59 seconds) 
    const char *sql = "SELECT * FROM RadioLogs WHERE time >= ? AND time <= ? ORDER BY time ASC;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, unixTime);
        sqlite3_bind_int64(stmt, 2, unixTime + 59); // 1-minute window
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            allResults += formatRowAsString(stmt); 
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return allResults.empty() ? "No results found for that time period." : allResults;
}