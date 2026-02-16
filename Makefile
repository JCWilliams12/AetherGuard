# Makefile for Windows (MinGW)

CC = gcc
CXX = g++

# --- PATHS ---
# CHANGED: No parentheses in folder name
SQL_DIR = database_methods

# 1. Include Paths
# Added -I so main.cpp can find "sqlite3.h" inside that folder
INCLUDES = -I./include -I$(SQL_DIR)

# 2. Compiler Flags
CFLAGS = -O2
CXXFLAGS = -std=c++17 -O2 $(INCLUDES) -DASIO_STANDALONE

# 3. Libraries
LIBS = -lws2_32 -lmswsock -lpthread

TARGET = main.exe

# --- TARGETS ---

all: $(TARGET)

# Link
$(TARGET): main.o sqlite3.o
	$(CXX) main.o sqlite3.o -o $(TARGET) $(LIBS)

# Compile C++ main
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

# Compile C sqlite3
# Rule: To make sqlite3.o, use the .c file in the folder
sqlite3.o: $(SQL_DIR)/sqlite3.c
	$(CC) $(CFLAGS) -c $(SQL_DIR)/sqlite3.c -o sqlite3.o

# Clean
clean:
	if exist *.o del *.o
	if exist $(TARGET) del $(TARGET)