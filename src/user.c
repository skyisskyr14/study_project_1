#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"
#include "common.h"
#include "log.h"

int user_register(MYSQL *conn) {
    char username[64], password[64], hash[65], sql[512];
    printf("请输入用户名(3-31位,字母数字下划线): ");
    read_line(username, sizeof(username));
    if (!is_valid_username(username)) {
        printf("用户名不合法。\n");
        return 0;
    }

    printf("请输入密码(6-32位,需包含字母和数字): ");
    read_line(password, sizeof(password));
    if (!is_valid_password(password)) {
        printf("密码复杂度不足。\n");
        return 0;
    }

    snprintf(sql, sizeof(sql), "SELECT id FROM users WHERE username='%s'", username);
    if (mysql_query(conn, sql) != 0) return 0;
    MYSQL_RES *res = mysql_store_result(conn);
    if (res && mysql_num_rows(res) > 0) {
        printf("用户名已存在。\n");
        mysql_free_result(res);
        return 0;
    }
    if (res) mysql_free_result(res);

    make_password_hash(password, hash);
    snprintf(sql, sizeof(sql),
             "INSERT INTO users(username,password_hash,user_type,has_voted) VALUES('%s','%s','voter',0)",
             username, hash);
    if (mysql_query(conn, sql) != 0) {
        printf("注册失败: %s\n", mysql_error(conn));
        return 0;
    }

    log_action(conn, username, "REGISTER", "SUCCESS");
    printf("注册成功。\n");
    return 1;
}

int user_login(MYSQL *conn, User *out_user) {
    char username[64], password[64], hash[65], sql[512];
    printf("用户名: ");
    read_line(username, sizeof(username));
    printf("密码: ");
    read_line(password, sizeof(password));
    make_password_hash(password, hash);

    snprintf(sql, sizeof(sql),
             "SELECT id,username,password_hash,user_type,has_voted,created_at "
             "FROM users WHERE username='%s'", username);
    if (mysql_query(conn, sql) != 0) {
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = res ? mysql_fetch_row(res) : NULL;
    if (!row) {
        printf("用户不存在。\n");
        if (res) mysql_free_result(res);
        log_action(conn, username, "LOGIN", "FAILED_NO_USER");
        return 0;
    }

    if (strcmp(hash, row[2]) != 0) {
        printf("密码错误。\n");
        mysql_free_result(res);
        log_action(conn, username, "LOGIN", "FAILED_BAD_PASSWORD");
        return 0;
    }

    out_user->id = atoi(row[0]);
    snprintf(out_user->username, sizeof(out_user->username), "%s", row[1]);
    snprintf(out_user->password_hash, sizeof(out_user->password_hash), "%s", row[2]);
    snprintf(out_user->user_type, sizeof(out_user->user_type), "%s", row[3]);
    out_user->has_voted = atoi(row[4]);
    snprintf(out_user->created_at, sizeof(out_user->created_at), "%s", row[5]);
    mysql_free_result(res);

    log_action(conn, out_user->username, "LOGIN", "SUCCESS");
    printf("登录成功，欢迎 %s。\n", out_user->username);
    return 1;
}

int user_change_password(MYSQL *conn, const User *current_user) {
    char old_pwd[64], new_pwd[64], old_hash[65], new_hash[65], sql[512];
    printf("请输入旧密码: "); read_line(old_pwd, sizeof(old_pwd));
    make_password_hash(old_pwd, old_hash);
    if (strcmp(old_hash, current_user->password_hash) != 0) {
        printf("旧密码错误。\n");
        log_action(conn, current_user->username, "CHANGE_PASSWORD", "FAILED_OLD_MISMATCH");
        return 0;
    }

    printf("请输入新密码: "); read_line(new_pwd, sizeof(new_pwd));
    if (!is_valid_password(new_pwd)) {
        printf("新密码复杂度不足。\n");
        return 0;
    }

    make_password_hash(new_pwd, new_hash);
    snprintf(sql, sizeof(sql), "UPDATE users SET password_hash='%s' WHERE id=%d", new_hash, current_user->id);
    if (mysql_query(conn, sql) != 0) {
        printf("修改失败: %s\n", mysql_error(conn));
        return 0;
    }
    log_action(conn, current_user->username, "CHANGE_PASSWORD", "SUCCESS");
    printf("密码修改成功。\n");
    return 1;
}

int user_mark_voted(MYSQL *conn, int user_id) {
    char sql[128];
    snprintf(sql, sizeof(sql), "UPDATE users SET has_voted=1 WHERE id=%d", user_id);
    return mysql_query(conn, sql) == 0;
}

int user_list_all(MYSQL *conn) {
    if (mysql_query(conn, "SELECT id,username,user_type,has_voted,created_at FROM users ORDER BY id") != 0) {
        return 0;
    }
    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) return 0;
    MYSQL_ROW row;
    printf("\nID\t用户名\t类型\t已投票\t创建时间\n");
    while ((row = mysql_fetch_row(res))) {
        printf("%s\t%s\t%s\t%s\t%s\n", row[0], row[1], row[2], row[3], row[4]);
    }
    mysql_free_result(res);
    return 1;
}
