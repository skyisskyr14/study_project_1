CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
SRC = $(wildcard src/*.c)
BIN = voting_system

MYSQL_CFLAGS := $(shell mysql_config --cflags 2>/dev/null)
MYSQL_LIBS := $(shell mysql_config --libs 2>/dev/null)

ifeq ($(strip $(MYSQL_CFLAGS)),)
MYSQL_CFLAGS := $(shell pkg-config --cflags libmysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs libmysqlclient 2>/dev/null)
endif

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -o $@ $^ $(MYSQL_LIBS)

clean:
	rm -f $(BIN)

.PHONY: all clean
