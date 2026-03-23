#include <stdlib.h>
#include "mysql_compat.h"

#ifdef MYSQL_STUB
static const char *STUB_ERR = "MySQL client library/header not found (stub mode)";

MYSQL *mysql_init(MYSQL *mysql) {
    if (mysql) return mysql;
    MYSQL *conn = (MYSQL *)malloc(sizeof(MYSQL));
    if (conn) conn->stub = 1;
    return conn;
}

MYSQL *mysql_real_connect(MYSQL *mysql, const char *host, const char *user,
                          const char *passwd, const char *db,
                          unsigned int port, const char *unix_socket,
                          unsigned long clientflag) {
    (void)host; (void)user; (void)passwd; (void)db;
    (void)port; (void)unix_socket; (void)clientflag;
    return mysql;
}

void mysql_close(MYSQL *sock) {
    free(sock);
}

int mysql_set_character_set(MYSQL *mysql, const char *csname) {
    (void)mysql; (void)csname;
    return 0;
}

int mysql_query(MYSQL *mysql, const char *q) {
    (void)mysql; (void)q;
    return 1;
}

MYSQL_RES *mysql_store_result(MYSQL *mysql) {
    (void)mysql;
    return NULL;
}

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result) {
    (void)result;
    return NULL;
}

void mysql_free_result(MYSQL_RES *result) {
    (void)result;
}

my_ulonglong mysql_num_rows(MYSQL_RES *res) {
    (void)res;
    return 0;
}

const char *mysql_error(MYSQL *mysql) {
    (void)mysql;
    return STUB_ERR;
}
#endif
