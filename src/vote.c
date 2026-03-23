#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vote.h"
#include "candidate.h"
#include "user.h"
#include "log.h"
#include "common.h"

static int is_in_voting_window(MYSQL *conn) {
    const char *sql =
        "SELECT "
        "MAX(CASE WHEN config_key='vote_start' THEN config_value END), "
        "MAX(CASE WHEN config_key='vote_end' THEN config_value END) "
        "FROM system_config";
    if (mysql_query(conn, sql) != 0) return 0;

    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = res ? mysql_fetch_row(res) : NULL;
    if (!row || !row[0] || !row[1]) {
        if (res) mysql_free_result(res);
        return 0;
    }

    char check_sql[512];
    snprintf(check_sql, sizeof(check_sql),
             "SELECT NOW() BETWEEN '%s' AND '%s'", row[0], row[1]);
    mysql_free_result(res);

    if (mysql_query(conn, check_sql) != 0) return 0;
    res = mysql_store_result(conn);
    row = res ? mysql_fetch_row(res) : NULL;
    int ok = row ? atoi(row[0]) : 0;
    if (res) mysql_free_result(res);
    return ok;
}

int vote_cast(MYSQL *conn, const User *current_user) {
    char sql[1024], input[32], ip[64], category[64];
    int candidate_id = 0;

    if (strcmp(current_user->user_type, "voter") != 0) {
        printf("仅普通用户可投票。\n");
        return 0;
    }
    if (!is_in_voting_window(conn)) {
        printf("当前不在投票时间窗口内。\n");
        return 0;
    }

    printf("输入本次投票分类(例如 Speaker / Lunch): ");
    read_line(category, sizeof(category));
    if (strlen(category) == 0) {
        printf("分类不能为空。\n");
        return 0;
    }

    if (!candidate_list(conn, "name", category)) {
        printf("该分类下暂无候选人或查询失败。\n");
        return 0;
    }

    printf("输入候选人 ID: ");
    read_line(input, sizeof(input));
    if (!to_int(input, &candidate_id)) {
        printf("ID 非法。\n");
        return 0;
    }

    printf("输入当前 IP (用于防刷票): ");
    read_line(ip, sizeof(ip));

    snprintf(sql, sizeof(sql),
             "SELECT COUNT(1) FROM candidates WHERE id=%d AND category='%s'",
             candidate_id, category);
    if (mysql_query(conn, sql) != 0) return 0;
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = res ? mysql_fetch_row(res) : NULL;
    int candidate_exists = row ? atoi(row[0]) : 0;
    if (res) mysql_free_result(res);
    if (!candidate_exists) {
        printf("候选人 ID 不属于该投票分类。\n");
        return 0;
    }

    snprintf(sql, sizeof(sql),
             "SELECT COUNT(1) FROM vote_records WHERE vote_category='%s' AND (user_id=%d OR ip_addr='%s')",
             category, current_user->id, ip);
    if (mysql_query(conn, sql) != 0) return 0;
    res = mysql_store_result(conn);
    row = res ? mysql_fetch_row(res) : NULL;
    int count = row ? atoi(row[0]) : 1;
    if (res) mysql_free_result(res);

    if (count > 0) {
        printf("检测到重复用户或 IP，禁止重复投票。\n");
        log_action(conn, current_user->username, "VOTE", "FAILED_DUPLICATE");
        return 0;
    }

    snprintf(sql, sizeof(sql),
             "INSERT INTO vote_records(user_id,candidate_id,vote_category,ip_addr) VALUES(%d,%d,'%s','%s')",
             current_user->id, candidate_id, category, ip);
    if (mysql_query(conn, sql) != 0) return 0;

    snprintf(sql, sizeof(sql),
             "UPDATE candidates SET vote_count=vote_count+1 WHERE id=%d", candidate_id);
    if (mysql_query(conn, sql) != 0) return 0;

    user_mark_voted(conn, current_user->id);
    log_action(conn, current_user->username, "VOTE", "SUCCESS");
    printf("投票成功。\n");
    return 1;
}

int vote_revoke(MYSQL *conn, const char *operator_name) {
    char input[32], category[64], sql[1024];
    int user_id = 0;
    printf("输入需要撤销投票的用户 ID: ");
    read_line(input, sizeof(input));
    if (!to_int(input, &user_id)) return 0;
    printf("输入需要撤销的投票分类: ");
    read_line(category, sizeof(category));
    if (strlen(category) == 0) {
        printf("分类不能为空。\n");
        return 0;
    }

    snprintf(sql, sizeof(sql),
             "SELECT candidate_id FROM vote_records WHERE user_id=%d AND vote_category='%s' LIMIT 1",
             user_id, category);
    if (mysql_query(conn, sql) != 0) return 0;
    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = res ? mysql_fetch_row(res) : NULL;
    if (!row) {
        if (res) mysql_free_result(res);
        printf("该用户无投票记录。\n");
        return 0;
    }
    int candidate_id = atoi(row[0]);
    mysql_free_result(res);

    snprintf(sql, sizeof(sql), "DELETE FROM vote_records WHERE user_id=%d AND vote_category='%s'", user_id, category);
    if (mysql_query(conn, sql) != 0) return 0;

    snprintf(sql, sizeof(sql), "UPDATE candidates SET vote_count=GREATEST(vote_count-1,0) WHERE id=%d", candidate_id);
    if (mysql_query(conn, sql) != 0) return 0;

    snprintf(sql, sizeof(sql),
             "UPDATE users SET has_voted=(SELECT CASE WHEN COUNT(1)>0 THEN 1 ELSE 0 END FROM vote_records WHERE user_id=%d) "
             "WHERE id=%d",
             user_id, user_id);
    if (mysql_query(conn, sql) != 0) return 0;

    log_action(conn, operator_name, "VOTE_REVOKE", "SUCCESS");
    printf("投票撤销成功。\n");
    return 1;
}
