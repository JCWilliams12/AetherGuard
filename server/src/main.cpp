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
    // ROUTE 1: GET ALL STATIONS
    // =======================================================
    CROW_ROUTE(app, "/stations")([](){
        std::vector<RadioLog> logs = getAllLogs();
        crow::json::wvalue response;
        
        // Prepare the JSON data first
        if (logs.empty()) {
            response = crow::json::wvalue(crow::json::type::List);
        } else {
            for (size_t i = 0; i < logs.size(); i++) {
                response[i]["id"] = i;
                response[i]["freq"] = std::to_string(logs[i].frequency) + " MHz";
                response[i]["time"] = logs[i].time;
                response[i]["location"] = logs[i].location;
                response[i]["name"] = logs[i].summary; 
            }
        }

        // Return the response object
        crow::response res(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res; 
    });


    // =======================================================
    // ROUTE 2: DELETE A LOG (Handles both OPTIONS and DELETE)
    // =======================================================
    CROW_ROUTE(app, "/api/logs/delete").methods(crow::HTTPMethod::Delete, crow::HTTPMethod::Options)
    ([](const crow::request& req) {
        
        // 1. Handle the browser's "pre-flight" security check
        if (req.method == crow::HTTPMethod::Options) {
            crow::response res(200); // "Yes, you are allowed"
            res.add_header("Access-Control-Allow-Origin", "*");
            res.add_header("Access-Control-Allow-Methods", "DELETE, OPTIONS");
            res.add_header("Access-Control-Allow-Headers", "Content-Type");
            return res;
        }

        // 2. Handle the actual DELETE request
        auto x = crow::json::load(req.body);
        if (!x) {
            crow::response res(400, "Bad JSON");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }

        // Extract variables and call your dbcorefunctions logic
        double freq = x["freq"].d();
        long long time = x["time"].i();
        removeLog(freq, time);

        // Send the success response back to React
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
    getAllLogs(); 
    
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