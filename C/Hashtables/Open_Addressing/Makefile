# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = test
UNITY_DIR = $(TEST_DIR)/unity

# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -I$(INC_DIR) -I$(UNITY_DIR)/src
CFLAGS_DEBUG = -DDEBUG_HASHTAB

# Source Files
LIB_SRCS = $(SRC_DIR)/open_addressing.c
TEST_SRCS = $(TEST_DIR)/test_open_addressing.c $(UNITY_DIR)/src/unity.c
MAIN_SRCS = $(SRC_DIR)/main.c

# Targets
LIB = libhashtable.a
TEST_EXEC = test_open_addressing
MAIN_EXEC = hashtable_main

# Object Files
LIB_OBJS = $(LIB_SRCS:.c=.o)
TEST_OBJS = $(TEST_SRCS:.c=.o)
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

# Headers
HEADERS = include/open_addressing.h test/unity/src/unity.h

# Phony Targets
.PHONY: all clean test

# Default Target: Build Library and Test Executable
all: $(LIB) $(TEST_EXEC) $(MAIN_EXEC)

# Build Static Library
$(LIB): $(LIB_OBJS)
	@echo "Creating static library $@..."
	ar rcs $@ $^

# Compile Object Files
%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Build Test Executable
$(TEST_EXEC): $(TEST_OBJS) $(LIB)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) -L. -lhashtable

# Build Main Executable
$(MAIN_EXEC): $(MAIN_OBJS) $(LIB)
	@echo "Linking $@..."
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJS) -L. -lhashtable

# Debug Build Target
debug: CFLAGS += $(CFLAGS_DEBUG)
debug: $(LIB) $(TEST_EXEC) $(MAIN_EXEC)

# Test Target: Run the Test Executable
test: $(TEST_EXEC)
	@echo "Running tests..."
	./$(TEST_EXEC)

# Clean Up Build Files
clean:
	@echo "Cleaning up..."
	rm -f $(LIB) $(LIB_OBJS) $(TEST_EXEC) $(TEST_OBJS) $(MAIN_EXEC) $(MAIN_OBJS)
