#For asio 
# 1. Detect OS and Architecture
UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)

# 2. Set Homebrew path based on Architecture (Mac specific)
ifeq ($(UNAME_S),Darwin)
    ifeq ($(UNAME_M),arm64)
        # Apple Silicon (M1/M2/M3)
        BREW_PREFIX = /opt/homebrew
    else
        # Intel Mac
        BREW_PREFIX = /usr/local
    endif
else
    # Default for Linux/other systems if brew isn't the primary manager
    BREW_PREFIX = /usr
endif

# Variables
CC = gcc
CXX = g++
CFLAGS = -O2
# Added pthread and boost flags for Crow
CXXFLAGS = -std=c++17 -O2 -I./include
LIBS = -lpthread -lboost_system


#for asio pt.2 
# Include the local ./include folder and the dynamic Homebrew folder
CXXFLAGS = -std=c++17 -O2 -I./include -I$(BREW_PREFIX)/include -DASIO_STANDALONE
# Link libraries from the dynamic Homebrew folder
LIBS = -L$(BREW_PREFIX)/lib -lpthread -lboost_system
# The final executable name (Linux standard is usually just 'main')
TARGET = main

# The object files
OBJS = main.o sqlite3.o

# Default target
all: $(TARGET)

# Linking the executable
# Added $(LIBS) here so Crow can use networking and threads
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compiling main.cpp as C++
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

# Compiling sqlite3.c as C
sqlite3.o: sqlite3.c
	$(CC) $(CFLAGS) -c sqlite3.c -o sqlite3.o

# Clean up build files
clean:
	rm -f *.o $(TARGET)