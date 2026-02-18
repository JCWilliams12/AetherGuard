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
    

    std::cout << "AetherGaurd running on port 8080..." << std::endl;
    app.port(8080).multithreaded().run();
}

int main(){
    //createTable();
    //openFrontEnd(); 
    return 0; 
}