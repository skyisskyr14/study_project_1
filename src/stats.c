#include <stdio.h>
#include <stdlib.h>
#include "stats.h"

int stats_show_report(MYSQL *conn) {
    const char *sql =
        "SELECT c.name, c.vote_count, "
        "CASE WHEN t.total_votes=0 THEN 0 ELSE ROUND(c.vote_count/t.total_votes*100,2) END AS rate "
        "FROM candidates c "
        "JOIN (SELECT IFNULL(SUM(vote_count),0) AS total_votes FROM candidates) t "
        "ORDER BY c.vote_count DESC";

    if (mysql_query(conn, sql) != 0) {
        fprintf(stderr, "统计查询失败: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) return 0;

    printf("\n============ 投票统计报表 ============\n");
    printf("候选人\t总票数\t得票率(%%)\n");
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        printf("%s\t%s\t%s\n", row[0], row[1], row[2]);
    }
    mysql_free_result(res);

    if (mysql_query(conn, "SELECT COUNT(*) FROM users WHERE user_type='voter'") == 0) {
        res = mysql_store_result(conn);
        row = res ? mysql_fetch_row(res) : NULL;
        int voter_total = row ? atoi(row[0]) : 0;
        if (res) mysql_free_result(res);

        mysql_query(conn, "SELECT COUNT(*) FROM users WHERE user_type='voter' AND has_voted=1");
        res = mysql_store_result(conn);
        row = res ? mysql_fetch_row(res) : NULL;
        int voted = row ? atoi(row[0]) : 0;
        if (res) mysql_free_result(res);

        printf("投票总人数: %d\n", voter_total);
        printf("已投票人数: %d\n", voted);
        printf("未投票人数: %d\n", voter_total - voted);
    }

    return 1;
}
