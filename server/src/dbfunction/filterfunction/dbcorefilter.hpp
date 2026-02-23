#ifndef DBCOREFILTERHPP
#define DBCOREFILTERHPP

#include <string>

// Global database path
extern const char* DB_NAME;

// Filter functions
std::string filterByFrequency(double freq);
std::string filterByTime(long long unixTime); // Updated name to match your .cpp
std::string filterByLocation(std::string loc);

#endif