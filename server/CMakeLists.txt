cmake_minimum_required(VERSION 3.10)
project(AetherGuardServer)

set(CMAKE_CXX_STANDARD 17)

# Suppress those annoying warnings about type conversion
if(MSVC)
    add_compile_options(/wd4267 /wd4244)
endif()

# Include directories
include_directories(include)
include_directories(src/db)

# Find Threads
find_package(Threads REQUIRED)

# Define the executable
add_executable(server 
    src/main.cpp
    src/db/sqlite3.c
)

# LINKING LOGIC (The Fix)
if(WIN32)
    # On Windows, link against Winsock (Ws2_32)
    target_link_libraries(server Threads::Threads Ws2_32)
else()
    # On Linux/Mac, link against dl
    target_link_libraries(server Threads::Threads dl)
endif()
