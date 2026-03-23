#include <stdio.h>
#include <string.h>
#include "log.h"
#include "common.h"

int log_action(MYSQL *conn, const char *operator_name, const char *action_type, const char *action_result) {
    char sql[512];
    snprintf(sql, sizeof(sql),
             "INSERT INTO logs(operator_name, action_type, action_result) VALUES('%s','%s','%s')",
             operator_name, action_type, action_result);
    if (mysql_query(conn, sql) != 0) {
        fprintf(stderr, "[LOG] write failed: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

int log_query(MYSQL *conn, const char *action_type_filter) {
    char sql[256];
    if (action_type_filter && strlen(action_type_filter) > 0) {
        snprintf(sql, sizeof(sql),
                 "SELECT id, operator_name, action_type, action_result, created_at "
                 "FROM logs WHERE action_type='%s' ORDER BY id DESC LIMIT 100", action_type_filter);
    } else {
        snprintf(sql, sizeof(sql),
                 "SELECT id, operator_name, action_type, action_result, created_at "
                 "FROM logs ORDER BY id DESC LIMIT 100");
    }

    if (mysql_query(conn, sql) != 0) {
        fprintf(stderr, "[LOG] query failed: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        return 0;
    }

    MYSQL_ROW row;
    printf("\n================ 日志查询结果 ================\n");
    printf("ID\t操作人\t类型\t结果\t时间\n");
    while ((row = mysql_fetch_row(res))) {
        printf("%s\t%s\t%s\t%s\t%s\n", row[0], row[1], row[2], row[3], row[4]);
    }
    mysql_free_result(res);
    return 1;
}
