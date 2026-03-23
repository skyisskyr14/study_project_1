CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
CORE_SRC = src/candidate.c src/common.c src/db.c src/log.c src/mysql_stub.c src/stats.c src/ui.c src/user.c src/vote.c
APP_SRC = $(CORE_SRC) src/main.c
SERVER_SRC = $(CORE_SRC) src/server.c
CLIENT_SRC = src/client.c
BIN = voting_system
SERVER_BIN = voting_server
CLIENT_BIN = voting_client

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

all: $(BIN) $(SERVER_BIN) $(CLIENT_BIN)

$(BIN): $(APP_SRC)
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -o $@ $^ $(MYSQL_LIBS)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -o $@ $^ $(MYSQL_LIBS)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(BIN) $(SERVER_BIN) $(CLIENT_BIN)

.PHONY: all clean
