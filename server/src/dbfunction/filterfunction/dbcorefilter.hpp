#ifndef DB_CORE_FUNCTIONS_HPP
#define DB_CORE_FUNCTIONS_HPP

#include <string>

// Export the DB_NAME so dbcorefilter.cpp can use it
extern const char* DB_NAME;

//initialize table 
void createTable();

void filterByFrequency(double freq); 
void filterByExactTime(long long targetTime);
void filterComposite(double freq, long long targetTime);

#endif