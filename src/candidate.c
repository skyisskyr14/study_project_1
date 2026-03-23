#include <stdio.h>
#include <string.h>
#include "candidate.h"
#include "common.h"
#include "log.h"

int candidate_add(MYSQL *conn, const char *operator_name) {
    char name[64], category[64], bio[256], sql[1024];
    printf("候选人姓名: "); read_line(name, sizeof(name));
    printf("候选人分类: "); read_line(category, sizeof(category));
    printf("候选人简介: "); read_line(bio, sizeof(bio));

    snprintf(sql, sizeof(sql),
             "INSERT INTO candidates(name,category,bio,vote_count) VALUES('%s','%s','%s',0)",
             name, category, bio);
    if (mysql_query(conn, sql) != 0) {
        printf("新增候选人失败: %s\n", mysql_error(conn));
        log_action(conn, operator_name, "CANDIDATE_ADD", "FAILED");
        return 0;
    }
    log_action(conn, operator_name, "CANDIDATE_ADD", "SUCCESS");
    printf("新增成功。\n");
    return 1;
}

int candidate_delete(MYSQL *conn, const char *operator_name) {
    char input[32], sql[256];
    int id = 0;
    printf("请输入删除候选人 ID: "); read_line(input, sizeof(input));
    if (!to_int(input, &id)) {
        printf("ID 输入非法。\n");
        return 0;
    }
    snprintf(sql, sizeof(sql), "DELETE FROM candidates WHERE id=%d", id);
    if (mysql_query(conn, sql) != 0) {
        log_action(conn, operator_name, "CANDIDATE_DELETE", "FAILED");
        return 0;
    }
    log_action(conn, operator_name, "CANDIDATE_DELETE", "SUCCESS");
    printf("删除成功。\n");
    return 1;
}

int candidate_update(MYSQL *conn, const char *operator_name) {
    char input[32], name[64], category[64], bio[256], sql[1024];
    int id = 0;
    printf("请输入候选人 ID: "); read_line(input, sizeof(input));
    if (!to_int(input, &id)) return 0;

    printf("新姓名: "); read_line(name, sizeof(name));
    printf("新分类: "); read_line(category, sizeof(category));
    printf("新简介: "); read_line(bio, sizeof(bio));

    snprintf(sql, sizeof(sql),
             "UPDATE candidates SET name='%s',category='%s',bio='%s' WHERE id=%d",
             name, category, bio, id);
    if (mysql_query(conn, sql) != 0) {
        log_action(conn, operator_name, "CANDIDATE_UPDATE", "FAILED");
        return 0;
    }
    log_action(conn, operator_name, "CANDIDATE_UPDATE", "SUCCESS");
    printf("修改成功。\n");
    return 1;
}

int candidate_list(MYSQL *conn, const char *order_field, const char *category_filter) {
    char sql[512];
    const char *order = (order_field && strcmp(order_field, "vote_count") == 0) ? "vote_count DESC" : "name ASC";
    if (category_filter && category_filter[0] != '\0') {
        snprintf(sql, sizeof(sql),
                 "SELECT id,name,category,bio,vote_count FROM candidates WHERE category='%s' ORDER BY %s",
                 category_filter, order);
    } else {
        snprintf(sql, sizeof(sql),
                 "SELECT id,name,category,bio,vote_count FROM candidates ORDER BY %s", order);
    }

    if (mysql_query(conn, sql) != 0) {
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) return 0;
    MYSQL_ROW row;
    printf("\nID\t姓名\t分类\t得票\t简介\n");
    while ((row = mysql_fetch_row(res))) {
        printf("%s\t%s\t%s\t%s\t%s\n", row[0], row[1], row[2], row[4], row[3]);
    }
    mysql_free_result(res);
    return 1;
}

int config_set_voting_window(MYSQL *conn, const char *operator_name) {
    char start_at[32], end_at[32], sql[512];
    printf("输入投票开始时间(YYYY-MM-DD HH:MM:SS): "); read_line(start_at, sizeof(start_at));
    printf("输入投票结束时间(YYYY-MM-DD HH:MM:SS): "); read_line(end_at, sizeof(end_at));

    snprintf(sql, sizeof(sql),
             "UPDATE system_config SET config_value='%s' WHERE config_key='vote_start';",
             start_at);
    if (mysql_query(conn, sql) != 0) return 0;

    snprintf(sql, sizeof(sql),
             "UPDATE system_config SET config_value='%s' WHERE config_key='vote_end';",
             end_at);
    if (mysql_query(conn, sql) != 0) return 0;

    log_action(conn, operator_name, "SET_VOTING_WINDOW", "SUCCESS");
    printf("投票时间窗口已更新。\n");
    return 1;
}
