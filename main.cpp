#include <iostream>
#include <string>
#include <stdio.h>
extern "C" {
    #include "database_methods\sqlite3.h"
}
#include "crow.h"

void openDatabase(){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("test.db", &db);

    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlite3_close(db);
}

void openFrontEnd(){
    crow::SimpleApp app;

    // Route for the left table (Stations)
    // Note that for future use we will be using CROW_ROUTE to direct data into certain tables.
    CROW_ROUTE(app, "/stations")([](){
        crow::json::wvalue x;
        x[0] = {{"id", 1}, {"name", "Emergency Dispatch"}, {"freq", "460.125 MHz"}};
        x[1] = {{"id", 2}, {"name", "Air Traffic Control"}, {"freq", "118.700 MHz"}};
        x[2] = {{"id", 3}, {"name", "Marine Channel 16"}, {"freq", "156.800 MHz"}};
        x[3] = {{"id", 4}, {"name", "Ham Radio (2m)"}, {"freq", "144.200 MHz"}};
        x[4] = {{"id", 5}, {"name", "Local Fire Dept"}, {"freq", "154.250 MHz"}};
        return x;
    });

    // Route for the right table (Logs/Summaries)
    CROW_ROUTE(app, "/logs")([](const crow::request& req){
        int stationId = 1; // Default
        
        // FIX: Replaced boost::lexical_cast with std::stoi
        if (req.url_params.get("id")) {
            try {
                stationId = std::stoi(req.url_params.get("id"));
            } catch (...) {
                stationId = 1; // Fallback if the ID isn't a number
            }
        }

        crow::json::wvalue logs;
        if (stationId == 1) {
            logs[0] = {{"id", 101}, {"date", "2026-02-02"}, {"summary", "Signal detected. Report of stalled vehicle on Highway 280."}};
            logs[1] = {{"id", 102}, {"date", "2026-02-02"}, {"summary", "Routine status check. All units clear."}};
        } else if (stationId == 2) {
            logs[0] = {{"id", 201}, {"date", "2026-02-02"}, {"summary", "Flight DL452 cleared for approach on Runway 24."}};
        } else {
            logs[0] = {{"id", 0}, {"date", "N/A"}, {"summary", "No logs found for this station."}};
        }
        return logs;
    });

    std::cout << "SDR Backend running on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();
}

int main(){
    openDatabase();
    openFrontEnd();
    return 0; 
}