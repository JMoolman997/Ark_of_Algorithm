CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
TARGET = hashtable
SRCS = open_addressing.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c open_addressing.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)
