cmake_minimum_required(VERSION 3.14) 
project(AetherGuardServer)

set(CMAKE_CXX_STANDARD 17)

# --- 0. DEPENDENCIES (Whisper.cpp) ---
include(FetchContent)
message(STATUS "Fetching whisper.cpp...")

FetchContent_Declare(
    whisper_cpp
    GIT_REPOSITORY https://github.com/ggerganov/whisper.cpp.git
    GIT_TAG        master
)

# Disable whisper's internal build targets
set(WHISPER_BUILD_TESTS OFF CACHE BOOL "Disable whisper tests" FORCE)
set(WHISPER_BUILD_EXAMPLES OFF CACHE BOOL "Disable whisper examples" FORCE)
set(WHISPER_BUILD_SERVER OFF CACHE BOOL "Disable whisper server" FORCE)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Force static libraries" FORCE)
FetchContent_MakeAvailable(whisper_cpp)

# --- 1. MODEL DOWNLOADER (Updated to download into source folder) ---
set(MODEL_FILENAME "ggml-base.en.bin")
set(MODEL_URL "https://huggingface.co/ggerganov/whisper.cpp/resolve/main/${MODEL_FILENAME}")

# TARGET PATH: Directly into your source code folder where the C++ code expects it
set(MODEL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/server/src/whispertinytest/${MODEL_FILENAME}")

if(NOT EXISTS "${MODEL_PATH}")
    message(STATUS "Model not found. Downloading ${MODEL_FILENAME} to ${MODEL_PATH}...")
    file(DOWNLOAD 
        ${MODEL_URL} 
        ${MODEL_PATH}
        SHOW_PROGRESS
        STATUS DOWNLOAD_STATUS
    )
    list(GET DOWNLOAD_STATUS 0 STATUS_CODE)
    if(NOT STATUS_CODE EQUAL 0)
        message(FATAL_ERROR "Failed to download model. Error: ${DOWNLOAD_STATUS}")
    endif()
else()
    message(STATUS "Whisper model found: ${MODEL_PATH}")
endif()

# --- 2. COMPILER OPTIONS ---
if(MSVC)
    add_compile_options(/wd4267 /wd4244)
endif()

# --- 3. INCLUDE DIRECTORIES (Updated for Root Context) ---
# All paths now have 'server/' prepended because this file sits in the root
include_directories(server/include)
include_directories(server/src/db)
include_directories(server/src/dbfunction/corefunction)
include_directories(server/src/dbfunction/filterfunction)
include_directories(server/src/iqwav2wav)
include_directories(server/src/ollamatest)
include_directories(server/src/openai/summarizer)
include_directories(server/src/openai/transcriber)
include_directories(server/src/sdr)
include_directories(server/src/whispertinytest)

find_package(Threads REQUIRED)

# --- 4. FIND SOURCES (Updated for Root Context) ---
# Looks for .cpp files inside server/src/
file(GLOB_RECURSE APP_SOURCES "server/src/*.cpp")

# --- 5. DEFINE EXECUTABLE ---
add_executable(server 
    ${APP_SOURCES}
    server/src/db/sqlite3.c
)

# --- 6. LINKING LOGIC ---
if(WIN32)
    target_link_libraries(server Threads::Threads Ws2_32 whisper)
else()
    target_link_libraries(server Threads::Threads dl whisper)
endif()

# --- 7. POST-BUILD ---
add_custom_command(TARGET server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${MODEL_PATH}"
    $<TARGET_FILE_DIR:server>
)