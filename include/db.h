#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>

/* 数据库配置结构体 */
typedef struct {
    char host[64];
    char user[64];
    char password[64];
    char database[64];
    unsigned int port;
} DbConfig;

/* 初始化并连接数据库 */
MYSQL *db_connect(const DbConfig *cfg);

/* 关闭数据库连接 */
void db_close(MYSQL *conn);

/* 执行无结果集 SQL */
int db_exec(MYSQL *conn, const char *sql);

/* 打印数据库错误 */
void db_print_error(MYSQL *conn, const char *scene);

#endif
