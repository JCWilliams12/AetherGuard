cmake_minimum_required(VERSION 3.10)
project(AetherGuardServer)

set(CMAKE_CXX_STANDARD 17)

# Suppress annoying warnings about type conversion
if(MSVC)
    add_compile_options(/wd4267 /wd4244)
endif()

# --- 1. INCLUDE DIRECTORIES (Where .h files live) ---
# This allows you to include headers without typing full paths
include_directories(include)
include_directories(src/db)
include_directories(src/dbfunction/corefunction)
include_directories(src/dbfunction/filterfunction)
include_directories(src/iqwav2wav)
include_directories(src/ollamatest)
include_directories(src/openai/summarizer)
include_directories(src/openai/transcriber)
include_directories(src/sdr)
include_directories(src/whispertinytest)


# Find Threads (Critical for Crow)
find_package(Threads REQUIRED)

# --- 2. FIND SOURCES (The "Auto-Discovery" Magic) ---
# This command finds ALL .cpp files inside src/ and its subfolders automatically.
file(GLOB_RECURSE APP_SOURCES "src/*.cpp")

# --- 3. DEFINE EXECUTABLE ---
add_executable(server 
    ${APP_SOURCES}      # Includes main.cpp and all your new folder implementations
    src/db/sqlite3.c    # SQLite is C, so we keep it separate
)

# --- 4. LINKING LOGIC ---
if(WIN32)
    # On Windows, link against Winsock (Ws2_32)
    target_link_libraries(server Threads::Threads Ws2_32)
else()
    # On Linux/Mac, link against dl
    target_link_libraries(server Threads::Threads dl)
endif()