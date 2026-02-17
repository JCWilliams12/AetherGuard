#ifndef DBCOREFUNCTIONS_HPP
#define DBCOREFUNCTIONS_HPP
#include <string>

void createTable();
void insertLog(double freq, long long time, std::string location, std::string text, std::string summary);
void getAllLogs();
void removeLog(double freq, long long time);

#endif