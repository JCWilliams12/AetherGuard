#ifndef DB_CORE_FUNCTIONS_HPP
#define DB_CORE_FUNCTIONS_HPP

#include <string>

// Export the DB_NAME so dbcorefilter.cpp can use it
extern const char* DB_NAME;

std::string filterByFrequency(double freq);
std::string filterByExactTime(long long targetTime);
std::string filterByDateString(std::string dateStr);
std::string filterCompositeExact(double freq, long long targetTime);
#endif