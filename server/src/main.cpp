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

    // Route for the left table (Stations)
    // Note that for future use we will be using CROW_ROUTE to direct data into certain tables.
CROW_ROUTE(app, "/stations")([](){
    std::vector<RadioLog> logs = getAllLogs();
    crow::json::wvalue response;
    
    // 1. Prepare the JSON data first
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

    // 2. NOW create the single response object and return it
    crow::response res(response);
    res.add_header("Access-Control-Allow-Origin", "*");
    return res; 
});
    std::cout << "AetherGuard running on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();
}

int main() {
    // Initialize DB table before starting server
    createTable();

    insertLog(144.200, 1718900000, "Montevallo, AL", "Testing signal strength", "Test Entry");
    // Launch the Crow server
    openFrontEnd();

    return 0;
}