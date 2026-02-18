#include <iostream>
#include <string>
#include <stdio.h>
#include "dbcorefunctions.hpp"
#include "dbcorefilter.hpp"
#include "crow.h"
#include "ollamatest.hpp"


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
    // filter by frequency
    CROW_ROUTE(app, "/filter_freq")([](const crow::request& req){
        auto mhzParam = req.url_params.get("mhz");
        if (mhzParam) {
            try {
                double freq = std::stod(mhzParam);
                filterByFrequency(freq); // Triggers DB logic -> writes to .txt
                return crow::response(200, "Frequency filter applied. Data written to file.");
            } catch (...) {
                return crow::response(400, "Invalid frequency format.");
            }
        }
        return crow::response(400, "Missing 'mhz' parameter.");
    });

    // LOGS ROUTE (Legacy/Static) 
    CROW_ROUTE(app, "/logs")([](const crow::request& req){
        int stationId = 1;
        if (req.url_params.get("id")) {
            try { stationId = std::stoi(req.url_params.get("id")); } catch (...) { stationId = 1; }
        }

        crow::json::wvalue logs;
        if (stationId == 1) {
            logs[0] = {{"id", 101}, {"date", "2026-02-02"}, {"summary", "Signal detected. Hwy 280."}};
        } else {
            logs[0] = {{"id", 0}, {"date", "N/A"}, {"summary", "No logs found."}};
        }
        return logs;
    });

    // filter by exact time
    CROW_ROUTE(app, "/filter_exact_time")([](const crow::request& req){
        auto tParam = req.url_params.get("t");
        if (tParam) {
            try {
                long long target = std::stoll(tParam);
                filterByExactTime(target); 
                return crow::response(200, "Exact time log found and written to file.");
            } catch (...) {
                return crow::response(400, "Invalid timestamp.");
            }
        }
        return crow::response(400, "Missing 't' parameter.");
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

    std::cout << "AetherGaurd running on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();
}

int main(){
    return 0; 
}