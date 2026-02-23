#include <iostream>
#include <string>
#include <stdio.h>
#include <vector>
#include "dbcorefunctions.hpp"
// #include "dbcorefilter.hpp" // Uncomment when you need these again!
#include "crow.h"
// #include "ollamatest.hpp"
// #include "whispertinytest.hpp"

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
        
        crow::json::wvalue json_data(station_list);
        crow::response res(json_data); 
        
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // =======================================================
    // ROUTE 1.5: GET SAVED LOGS (For the Database View)
    // =======================================================
    CROW_ROUTE(app, "/api/logs")([](){
        std::vector<RadioLog> logs = getAllLogs();
        crow::json::wvalue response;
        
        if (logs.empty()) {
            response = crow::json::wvalue(crow::json::type::List);
        } else {
            for (size_t i = 0; i < logs.size(); i++) {
                response[i]["id"] = i; 
                response[i]["freq"] = std::to_string(logs[i].freq); // Updated to .freq
                response[i]["time"] = logs[i].time;
                response[i]["location"] = logs[i].location;
                // Map the C++ 'channelName' to React's 'name'
                response[i]["name"] = logs[i].channelName; 
                // Pass the summary and raw text to React!
                response[i]["summary"] = logs[i].summary;
                response[i]["rawT"] = logs[i].rawT;
            }
        }

        crow::response res(response);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res; 
    });

    // =======================================================
    // ROUTE 2: DELETE A LOG 
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

        // Extract variables, INCLUDING LOCATION NOW!
        double freq = x.has("freq") ? x["freq"].d() : 0.0;
        long long time = x.has("time") ? x["time"].i() : 0;
        std::string location = x.has("location") ? std::string(x["location"].s()) : "";

        std::cout << "Executing removeLog(" << freq << ", " << time << ", " << location << ")..." << std::endl;
        
        // Pass all 3 keys to removeLog
        removeLog(freq, time, location);

        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // =======================================================
    // ROUTE 2.5: SAVE A LOG (Brand New!)
    // =======================================================
    CROW_ROUTE(app, "/api/logs/save").methods(crow::HTTPMethod::Post)
    ([](const crow::request& req) {
        
        std::cout << "\n--- INCOMING SAVE REQUEST ---" << std::endl;
        
        auto x = crow::json::load(req.body);
        if (!x) {
            crow::response res(400, "Bad JSON");
            res.add_header("Access-Control-Allow-Origin", "*");
            return res;
        }

        // Extract all 6 fields from React
        double freq = x.has("freq") ? x["freq"].d() : 0.0;
        long long time = x.has("time") ? x["time"].i() : 0;
        std::string location = x.has("location") ? std::string(x["location"].s()) : "Unknown";
        std::string rawT = x.has("rawT") ? std::string(x["rawT"].s()) : "No raw text provided.";
        std::string summary = x.has("summary") ? std::string(x["summary"].s()) : "No summary available.";
        std::string channelName = x.has("channelName") ? std::string(x["channelName"].s()) : "Unknown Station";
        
        insertLog(freq, time, location, rawT, summary, channelName);

        crow::response res(200);
        res.add_header("Access-Control-Allow-Origin", "*");
        return res;
    });

    // =======================================================
    // ROUTE 3: CATCH-ALL 404 LOGGER
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

    // UPDATED SIGNATURE: freq, time, location, rawT, summary, channelName
    insertLog(144.200, 1718900000, "Birmingham, AL", "[Raw Audio Data]", "Testing signal strength", "Test Station");
    
    // Launch the Crow server
    openFrontEnd();
    
    return 0;
}