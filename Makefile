# --- OS Detection ---
ifeq ($(OS),Windows_NT)
    # Windows
    TARGET = main.exe
    LIBS = -lws2_32 -lmswsock -lpthread
    CLEAN_CMD = if exist *.o del *.o && if exist $(TARGET) del $(TARGET)
else
    # Mac/Linux check
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # macOS
        TARGET = main
        LIBS = -lpthread -ldl
        CLEAN_CMD = rm -f *.o $(TARGET)
    else
        # Linux
        TARGET = main
        LIBS = -lpthread -ldl
        CLEAN_CMD = rm -f *.o $(TARGET)
    endif
endif

# --- Compilers ---
CC = gcc
CXX = g++

# --- PATHS ---
SQL_DIR = database_methods
INCLUDES = -I./include -I$(SQL_DIR)

# --- Flags ---
CFLAGS = -O2
CXXFLAGS = -std=c++17 -O2 $(INCLUDES) -DASIO_STANDALONE

# --- TARGETS ---
all: $(TARGET)

# Link
$(TARGET): main.o sqlite3.o
	$(CXX) main.o sqlite3.o -o $(TARGET) $(LIBS)

# Compile C++ main
main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

# Compile C sqlite3
sqlite3.o: $(SQL_DIR)/sqlite3.c
	$(CC) $(CFLAGS) -c $(SQL_DIR)/sqlite3.c -o sqlite3.o

# Clean
clean:
	$(CLEAN_CMD)