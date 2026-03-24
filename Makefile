CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
CORE_SRC = src/candidate.c src/common.c src/db.c src/log.c src/mysql_stub.c src/stats.c src/ui.c src/user.c src/vote.c
SERVER_SRC = $(CORE_SRC) src/server.c
CLIENT_SRC = src/client.c
SERVER_BIN = voting_server
CLIENT_BIN = voting_client

# 先探测 mysql_config / mariadb_config，再尝试 pkg-config；都失败才进入 stub 模式
MYSQL_CFLAGS := $(shell command -v mysql_config >/dev/null 2>&1 && mysql_config --cflags)
MYSQL_LIBS := $(shell command -v mysql_config >/dev/null 2>&1 && mysql_config --libs)

ifeq ($(strip $(MYSQL_CFLAGS)),)
MYSQL_CFLAGS := $(shell command -v mariadb_config >/dev/null 2>&1 && mariadb_config --cflags)
MYSQL_LIBS := $(shell command -v mariadb_config >/dev/null 2>&1 && mariadb_config --libs)
endif

ifeq ($(strip $(MYSQL_CFLAGS)),)
MYSQL_CFLAGS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --cflags libmysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --libs libmysqlclient 2>/dev/null)
endif

ifeq ($(strip $(MYSQL_CFLAGS)),)
MYSQL_CFLAGS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --cflags mariadb 2>/dev/null)
MYSQL_LIBS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --libs mariadb 2>/dev/null)
endif

ifeq ($(strip $(MYSQL_CFLAGS)),)
MYSQL_CFLAGS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --cflags mariadbclient 2>/dev/null)
MYSQL_LIBS := $(shell command -v pkg-config >/dev/null 2>&1 && pkg-config --libs mariadbclient 2>/dev/null)
endif

# 某些 Debian/Ubuntu 环境没有 config 脚本，但头文件和库已安装在系统默认路径
ifeq ($(strip $(MYSQL_CFLAGS)),)
ifneq ($(wildcard /usr/include/mysql/mysql.h),)
MYSQL_CFLAGS := -I/usr/include/mysql
MYSQL_LIBS := -lmysqlclient
else ifneq ($(wildcard /usr/include/mariadb/mysql.h),)
MYSQL_CFLAGS := -I/usr/include/mariadb
MYSQL_LIBS := -lmariadb
else ifneq ($(wildcard /usr/include/mariadb/server/mysql.h),)
MYSQL_CFLAGS := -I/usr/include/mariadb/server
MYSQL_LIBS := -lmariadb
endif
endif

ifeq ($(strip $(MYSQL_CFLAGS)),)
ifeq ($(ALLOW_STUB),1)
CFLAGS += -DFORCE_MYSQL_STUB
$(info [build] 未检测到 MySQL/MariaDB 开发库，按 ALLOW_STUB=1 使用 stub 模式编译)
else
$(error 未检测到 MySQL/MariaDB 开发库，请安装 libmariadb-dev 或 libmysqlclient-dev；若只做演示可使用 make ALLOW_STUB=1)
endif
else
$(info [build] 使用真实 MySQL/MariaDB 库编译)
endif

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(MYSQL_CFLAGS) -o $@ $^ $(MYSQL_LIBS)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

.PHONY: all clean
