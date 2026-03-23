#include <stdio.h>
#include "db.h"

MYSQL *db_connect(const DbConfig *cfg) {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "[DB] mysql_init failed\n");
        return NULL;
    }

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
