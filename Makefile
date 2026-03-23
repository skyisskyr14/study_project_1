CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
SRC = $(wildcard src/*.c)
BIN = voting_system

# 先探测 mysql_config，再尝试 pkg-config；都失败则进入 stub 模式
MYSQL_CFLAGS := $(shell command -v mysql_config >/dev/null 2>&1 && mysql_config --cflags)
MYSQL_LIBS := $(shell command -v mysql_config >/dev/null 2>&1 && mysql_config --libs)

ifeq ($(strip $(MYSQL_CFLAGS)),)
MYSQL_CFLAGS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --cflags libmysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --libs libmysqlclient 2>/dev/null)
endif

ifeq ($(strip $(MYSQL_CFLAGS)),)
CFLAGS += -DFORCE_MYSQL_STUB
endif

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -o $@ $^ $(MYSQL_LIBS)

clean:
	rm -f $(BIN)

.PHONY: all clean
