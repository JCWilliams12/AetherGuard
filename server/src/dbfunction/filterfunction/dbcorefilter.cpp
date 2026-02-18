#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <ctime>
#include "dbcorefilter.hpp"
#include "../corefunction/dbcorefunctions.hpp"

extern "C" {
    #include "sqlite3.h"
}
//keys are freq and time
// Helper where the logic is to write filtered data to the exchange file
void writeLogToFile(sqlite3_stmt* stmt) {
    std::ofstream outFile("filtered_results.txt", std::ios::app);
    if (!outFile.is_open()) return;

    double freq = sqlite3_column_double(stmt, 0);
    long long rawTime = sqlite3_column_int64(stmt, 1);
    const char* loc = (const char*)sqlite3_column_text(stmt, 2);
    const char* txt = (const char*)sqlite3_column_text(stmt, 3);
    const char* sum = (const char*)sqlite3_column_text(stmt, 4);

    time_t tick = (time_t)rawTime;
    struct tm *tm_info = localtime(&tick);
    char timeBuffer[26];
    strftime(timeBuffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    outFile << "-------------------------------------------" << std::endl;
    outFile << "FREQ: " << freq << " MHz | TIME: " << timeBuffer << std::endl;
    outFile << "LOC:  " << (loc ? loc : "N/A") << std::endl;
    outFile << "TEXT: " << (txt ? txt : "N/A") << std::endl;
    outFile << "SUMM: " << (sum ? sum : "N/A") << std::endl;
    outFile.close();
}

// Helper that resets the exchange file for a new query result
void clearResultsFile() {
    std::ofstream outFile("filtered_results.txt", std::ios::trunc);
    outFile.close();
}

// Filter by frequency 
void filterByFrequency(double freq) {
    clearResultsFile(); 
    sqlite3 *db;
    sqlite3_stmt *stmt;
    
    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return;

    const char *sql = "SELECT * FROM RadioLogs WHERE radiofrequency = ? ORDER BY time DESC;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, freq);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            writeLogToFile(stmt);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

// Filter by exact time 
void filterByExactTime(long long targetTime) {
    clearResultsFile(); 
    sqlite3 *db;
    sqlite3_stmt *stmt;
    
    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return;

    const char *sql = "SELECT * FROM RadioLogs WHERE time = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_int64(stmt, 1, targetTime);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            writeLogToFile(stmt);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

// Filter by frequency and exact time 
void filterCompositeExact(double freq, long long targetTime) {
    clearResultsFile();
    sqlite3 *db;
    sqlite3_stmt *stmt;
    
    if (sqlite3_open(DB_NAME, &db) != SQLITE_OK) return;

    const char *sql = "SELECT * FROM RadioLogs WHERE radiofrequency = ? AND time = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_double(stmt, 1, freq);
        sqlite3_bind_int64(stmt, 2, targetTime);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            writeLogToFile(stmt);
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}