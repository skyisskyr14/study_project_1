#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "db.h"
#include "user.h"
#include "vote.h"
#include "candidate.h"
#include "stats.h"
#include "log.h"
#include "common.h"

static DbConfig g_db = {
    .host = "localhost",
    .user = "root",
    .password = "",
    .database = "voting_system",
    .port = 3306
};

static void load_db_config_from_env(void) {
    const char *host = getenv("VOTE_DB_HOST");
    const char *user = getenv("VOTE_DB_USER");
    const char *password = getenv("VOTE_DB_PASSWORD");
    const char *database = getenv("VOTE_DB_NAME");
    const char *port = getenv("VOTE_DB_PORT");

    if (host && host[0] != '\0') {
        snprintf(g_db.host, sizeof(g_db.host), "%s", host);
    }
    if (user && user[0] != '\0') {
        snprintf(g_db.user, sizeof(g_db.user), "%s", user);
    }
    if (password) {
        snprintf(g_db.password, sizeof(g_db.password), "%s", password);
    }
    if (database && database[0] != '\0') {
        snprintf(g_db.database, sizeof(g_db.database), "%s", database);
    }
    if (port && port[0] != '\0') {
        unsigned long p = strtoul(port, NULL, 10);
        if (p > 0 && p <= 65535) {
            g_db.port = (unsigned int)p;
        }
    }
}

static void show_guest_menu(void) {
    printf("\n========== 投票系统 =========="
           "\n1. 注册"
           "\n2. 登录"
           "\n0. 退出"
           "\n请选择: ");
}

void run_admin_menu(void *db_conn, const User *current_user) {
    MYSQL *conn = (MYSQL *)db_conn;
    char input[32];
    int choice = -1;

    while (1) {
        printf("\n------ 管理员菜单 ------"
               "\n1. 添加候选人"
               "\n2. 删除候选人"
               "\n3. 修改候选人"
               "\n4. 查看候选人(按名称)"
               "\n5. 查看候选人(按得票)"
               "\n6. 设置投票时间"
               "\n7. 撤销投票"
               "\n8. 查看用户列表"
               "\n9. 查看统计报表"
               "\n10. 查询日志"
               "\n0. 退出登录"
               "\n请选择: ");
        read_line(input, sizeof(input));
        if (!to_int(input, &choice)) continue;

        switch (choice) {
            case 1: candidate_add(conn, current_user->username); break;
            case 2: candidate_delete(conn, current_user->username); break;
            case 3: candidate_update(conn, current_user->username); break;
            case 4: candidate_list(conn, "name", NULL); break;
            case 5: candidate_list(conn, "vote_count", NULL); break;
            case 6: config_set_voting_window(conn, current_user->username); break;
            case 7: vote_revoke(conn, current_user->username); break;
            case 8: user_list_all(conn); break;
            case 9: stats_show_report(conn); break;
            case 10: {
                char filter[32];
                printf("输入日志类型(空为全部): ");
                read_line(filter, sizeof(filter));
                log_query(conn, filter);
                break;
            }
            case 0: return;
            default: printf("无效选项。\n");
        }
    }
}

void run_voter_menu(void *db_conn, const User *current_user) {
    MYSQL *conn = (MYSQL *)db_conn;
    User session_user = *current_user;
    char input[32];
    int choice = -1;

    while (1) {
        printf("\n------ 用户菜单 ------"
               "\n1. 查看候选人(按名称)"
               "\n2. 查看候选人(按得票)"
               "\n3. 进行投票"
               "\n4. 修改密码"
               "\n5. 查看统计报表"
               "\n0. 退出登录"
               "\n请选择: ");
        read_line(input, sizeof(input));
        if (!to_int(input, &choice)) continue;

        switch (choice) {
            case 1: candidate_list(conn, "name", NULL); break;
            case 2: candidate_list(conn, "vote_count", NULL); break;
            case 3:
                if (vote_cast(conn, &session_user)) {
                    session_user.has_voted = 1;
                }
                break;
            case 4: user_change_password(conn, &session_user); break;
            case 5: stats_show_report(conn); break;
            case 0: return;
            default: printf("无效选项。\n");
        }
    }
}

void run_app(void) {
    load_db_config_from_env();
    MYSQL *conn = db_connect(&g_db);
    if (!conn) {
        printf("数据库连接失败，请检查配置与 MySQL 服务。\n");
        printf("当前连接配置: host=%s user=%s db=%s port=%u\n",
               g_db.host, g_db.user, g_db.database, g_db.port);
        printf("可通过环境变量覆盖: VOTE_DB_HOST / VOTE_DB_USER / VOTE_DB_PASSWORD / VOTE_DB_NAME / VOTE_DB_PORT\n");
        return;
    }

    char input[32];
    int choice = -1;
    while (1) {
        show_guest_menu();
        read_line(input, sizeof(input));
        if (!to_int(input, &choice)) continue;

        if (choice == 1) {
            user_register(conn);
        } else if (choice == 2) {
            User current_user;
            if (user_login(conn, &current_user)) {
                if (strcmp(current_user.user_type, "admin") == 0) {
                    run_admin_menu(conn, &current_user);
                } else {
                    run_voter_menu(conn, &current_user);
                }
            }
        } else if (choice == 0) {
            break;
        } else {
            printf("无效选项。\n");
        }
    }

    db_close(conn);
}
