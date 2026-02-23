#include <iostream>
#include <string>
#include <stdio.h>
#include "dbcorefunctions.hpp"
#include "dbcorefilter.hpp"
#include "crow.h"
#include "ollamatest.hpp"
#include "whispertinytest.hpp"


void openFrontEnd(){
    crow::SimpleApp app;

    // =======================================================
    // ROUTE 1: GET LIVE STATIONS (For the Scanning View)
    // =======================================================
    CROW_ROUTE(app, "/stations")([](){
        // NOTE: Replace this mock data with your actual live scanner logic later!
        crow::json::wvalue station1 = {{"id", 1}, {"name", "Live Scanner 1"}, {"freq", "144.200"}};
        crow::json::wvalue station2 = {{"id", 2}, {"name", "Live Scanner 2"}, {"freq", "155.100"}};
        
        std::vector<crow::json::wvalue> station_list = {station1, station2};
        
        // FIX: Create the JSON object first, THEN pass it to the response
        crow::json::wvalue json_data(station_list);
        crow::response res(json_data); 
        
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // =======================================================
    // ROUTE 1.5: GET SAVED LOGS (For the Database View)
    // =======================================================
    CROW_ROUTE(app, "/api/logs")([](){
        // This pulls from your sqlite database!
        std::vector<RadioLog> logs = getAllLogs();
        crow::json::wvalue response;
        
        if (logs.empty()) {
            response = crow::json::wvalue(crow::json::type::List);
        } else {
            for (size_t i = 0; i < logs.size(); i++) {
                response[i]["id"] = i; 
                // Removed the " MHz" text here so React's parseFloat() in the delete function works perfectly
                response[i]["freq"] = std::to_string(logs[i].frequency); 
                response[i]["time"] = logs[i].time;
                response[i]["location"] = logs[i].location;
                response[i]["name"] = logs[i].summary; 
            }
        }

        crow::response res(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res; 
    });


    // =======================================================
    // ROUTE 2: DELETE A LOG (Using POST to bypass CORS)
    // =======================================================
    CROW_ROUTE(app, "/api/logs/delete").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        
        std::cout << "\n--- INCOMING DELETE REQUEST ---" << std::endl;
        
        auto x = crow::json::load(req.body);
        if (!x) {
            crow::response res(400, "Bad JSON");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }

        // Extract variables
        double freq = x.has("freq") ? x["freq"].d() : 0.0;
        long long time = x.has("time") ? x["time"].i() : 0;

        std::cout << "Executing removeLog(" << freq << ", " << time << ")..." << std::endl;
        
        // Actually call the database function
        removeLog(freq, time);

        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });


    // =======================================================
    // ROUTE 3: CATCH-ALL 404 LOGGER (For debugging)
    // =======================================================
    app.catchall_route()([](const crow::request& req, crow::response& res) {
        std::cout << "\n[404 DEBUG] Frontend asked for URL: " << req.url 
                  << " | Method: " << crow::method_name(req.method) << std::endl;
        
        res.code = 404;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.body = "Route completely missed";
        res.end();
    });


    std::cout << "AetherGuard running on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();
}

int main() {
    
    // Initialize DB table before starting server
    createTable();

    insertLog(144.200, 1718900000, "Birmingham, AL", "Testing signal strength", "Test Entry");
    // Launch the Crow server
    openFrontEnd();
    
    return 0;
}

/*
William's: 
// Initialize DB table before starting server
    createTable();

    insertLog(144.200, 1718900000, "Montevallo, AL", "Testing signal strength", "Test Entry");
    // Launch the Crow server
Daniel's
    // 1. Simulate a "Filter by Frequency" Button Click
    std::cout << "--- EVENT: User clicked 144.20 MHz ---" << std::endl;
    std::string freqResult = filterByFrequency(144.20);
    std::cout << freqResult << std::endl;

    // 2. Simulate a "Location Search" Event
    std::cout << "--- EVENT: User searched for 'London' ---" << std::endl;
    std::string locResult = filterByLocation("Motevallo");
    std::cout << locResult << std::endl;

    // 3. Simulate a "Time" selection (Unix for Feb 23, 2026, 1:30 PM)
    // In a real app, this unixTime would come from your UI clock
    long long mockUnixTime = 1718900000; 
    std::cout << "--- EVENT: User selected timestamp " << mockUnixTime << " ---" << std::endl;
    std::string timeResult = filterByTime(mockUnixTime);
    std::cout << timeResult << std::endl;

    openFrontEnd();

*/