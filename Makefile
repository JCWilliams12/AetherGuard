# Variables
CC = gcc
CXX = g++
CFLAGS = -O2
CXXFLAGS = -std=c++17 -O2

# The final executable name
TARGET = main.exe

# The object files
OBJS = main.o sqlite3.o

# Default target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Compiling main.cpp as C++
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

# Compiling sqlite3.c as C
sqlite3.o: sqlite3.c
	$(CC) $(CFLAGS) -c sqlite3.c -o sqlite3.o

# Clean up build files
clean:
	del *.o *.exe