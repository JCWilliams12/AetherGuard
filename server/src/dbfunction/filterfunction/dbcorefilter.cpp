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
//Helper to conver YYYY - MM - DD HH:MM:SS to a unix time stamp 
long long dateStringToUnix(std::string dateStr) {
    struct tm t = {0};
    std::istringstream ss(dateStr);
    ss >> std::get_time(&t, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        return -1; 
    }
    return (long long)mktime(&t);
}

// Filter by frequency 
std::string filterByFrequency(double freq) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string allResults = "";

    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return "Error: Could not open database.";

    const char *sql = "SELECT * FROM RadioLogs WHERE radiofrequency = ? ORDER BY time DESC;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, freq);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            allResults += formatRowAsString(stmt); 
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return allResults.empty() ? "No results found for that frequency." : allResults;
}

// Filter by exact time 
std::string filterByExactTime(long long targetTime) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string allResults = "";

    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return "Error: Could not open database.";

    const char *sql = "SELECT * FROM RadioLogs WHERE time = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, targetTime);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            allResults += formatRowAsString(stmt); 
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return allResults.empty() ? "No results found for that timestamp." : allResults;
}

// Filter by date string 
std::string filterByDateString(std::string dateStr) {
    long long timestamp = dateStringToUnix(dateStr);
    
    if (timestamp != -1) {
        return filterByExactTime(timestamp);
    } else {
        return "Error: Invalid date format. Please use YYYY-MM-DD HH:MM:SS";
    }
}

// Filter by frequency and exact time 
std::string filterCompositeExact(double freq, long long targetTime) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    std::string allResults = "";

    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return "Error: Could not open database.";

    const char *sql = "SELECT * FROM RadioLogs WHERE radiofrequency = ? AND time = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, freq);
        sqlite3_bind_int64(stmt, 2, targetTime);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            allResults += formatRowAsString(stmt);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return allResults.empty() ? "No matching composite record found." : allResults;
}