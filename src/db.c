#include <stdio.h>
#include "db.h"

MYSQL *db_connect(const DbConfig *cfg) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "[DB] mysql_init failed\n");
        return NULL;
    }

#ifdef MYSQL_STUB
    fprintf(stderr, "[DB] 当前为 MySQL stub 模式：未检测到 mysql/mysql.h 或客户端库。\n");
    fprintf(stderr, "[DB] 请安装 MySQL 开发库后重新编译，或继续用于界面演示。\n");
    mysql_close(conn);
    return NULL;
#endif

    if (!mysql_real_connect(conn, cfg->host, cfg->user, cfg->password,
                            cfg->database, cfg->port, NULL, 0)) {
        fprintf(stderr, "[DB] connect failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    mysql_set_character_set(conn, "utf8mb4");
    return conn;
}

void db_close(MYSQL *conn) {
    if (conn) {
        mysql_close(conn);
    }
}

int db_exec(MYSQL *conn, const char *sql) {
    if (mysql_query(conn, sql) != 0) {
        return 0;
    }
    return 1;
}

void db_print_error(MYSQL *conn, const char *scene) {
    fprintf(stderr, "[DB][%s] %s\n", scene, mysql_error(conn));
}
