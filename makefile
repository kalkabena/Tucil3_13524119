# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -Isrc
SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:src/%.c=bin/%.o)

ifeq ($(OS),Windows_NT)
	TARGET = bin/game_solver.exe
	MKDIR = if not exist bin mkdir bin
	CLEAN = del /Q bin\*.o $(TARGET) 2>nul || cd .
else
	TARGET = bin/game_solver
	MKDIR = mkdir -p bin
	CLEAN = rm -f bin/*.o $(TARGET)
endif

# ---------------------------------------------------------
# Targets
# ---------------------------------------------------------

# Default target: compile the program
.PHONY: all
all: $(TARGET)

# Link all object files into the final executable
 $(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile individual .c files into .o files in the bin/ directory
bin/%.o: src/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Run the compiled program
.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Clean up compiled files
.PHONY: clean
clean:
	$(CLEAN)