#ifndef MYSQL_COMPAT_H
#define MYSQL_COMPAT_H

/*
 * MySQL 兼容层：
 * - 有 mysql/mysql.h 时使用真实 MySQL C API
 * - 无开发头文件时启用 stub，保证代码可编译（运行时提示不可用）
 */
#if defined(FORCE_MYSQL_STUB)
#define MYSQL_STUB 1
#elif defined(__has_include)
#  if __has_include(<mysql/mysql.h>)
#    include <mysql/mysql.h>
#  else
#    define MYSQL_STUB 1
#  endif
#else
#define MYSQL_STUB 1
#endif

#ifdef MYSQL_STUB
typedef unsigned long long my_ulonglong;

typedef struct MYSQL {
    int stub;
} MYSQL;

typedef struct MYSQL_RES {
    int stub;
} MYSQL_RES;

typedef char **MYSQL_ROW;

MYSQL *mysql_init(MYSQL *mysql);
MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user,
                          const char *passwd, const char *db,
                          unsigned int port, const char *unix_socket,
                          unsigned long clientflag);
void mysql_close(MYSQL *sock);
int mysql_set_character_set(MYSQL *mysql, const char *csname);
int mysql_query(MYSQL *mysql, const char *q);
MYSQL_RES *mysql_store_result(MYSQL *mysql);
MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
void mysql_free_result(MYSQL_RES *result);
my_ulonglong mysql_num_rows(MYSQL_RES *res);
const char *mysql_error(MYSQL *mysql);
#endif

#endif
