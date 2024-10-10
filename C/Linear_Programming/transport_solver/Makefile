# ============================
# Makefile for Transportation Tool
# ============================

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -g -I$(SRCDIR)

# Directories
SRCDIR = src
BUILDDIR = build
BINDIR = bin

# Name of the final executable
TARGET = transport_genie

# Find all .c files in the src directory
SRCS = $(wildcard $(SRCDIR)/*.c)

# Generate object file names by replacing src/ with build/ and .c with .o
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

# Default target: build the executable
all: $(BINDIR)/$(TARGET)

# Rule to link object files into the final executable
$(BINDIR)/$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

# Generic rule to compile .c files into .o files
# $< is the source file, $@ is the target object file
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Create bin directory if it doesn't exist
$(BINDIR):
	mkdir -p $(BINDIR)

# Clean target to remove build and bin directories
clean:
	rm -rf $(BUILDDIR) $(BINDIR)

# Phony targets to prevent conflicts with files named 'all' or 'clean'
.PHONY: all clean
