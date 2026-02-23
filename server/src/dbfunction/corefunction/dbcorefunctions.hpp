#ifndef DBCOREFUNCTIONS_HPP
#define DBCOREFUNCTIONS_HPP
#include <string>
#include <vector>

// The "Container" for your radio data
struct RadioLog {
    double frequency;
    long long time;
    std::string location;
    std::string text;
    std::string summary;
};

void createTable();
void insertLog(double freq, long long time, std::string location, std::string text, std::string summary);
int removeLog(double freq, long long time);
void openDatabase();

// Updated to return a vector of the struct above
std::vector<RadioLog> getAllLogs();

#endif

