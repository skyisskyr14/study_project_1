#ifndef COMMON_H
#define COMMON_H

#include <mysql/mysql.h>
#include "models.h"

#define APP_NAME "Terminal Voting System"
#define APP_VERSION "1.0.0"

/* 统一输入读取，自动去除行尾换行符 */
void read_line(char *buffer, int size);

/* 简单哈希（演示用途，生产建议使用 bcrypt/argon2） */
void make_password_hash(const char *password, char out_hash[65]);

/* 验证用户名是否合法（长度与字符限制） */
int is_valid_username(const char *username);

/* 验证密码复杂度（最小长度、字母数字组合） */
int is_valid_password(const char *password);

/* 获取当前时间字符串（YYYY-MM-DD HH:MM:SS） */
void now_str(char out[20]);

/* 输入安全转换整型 */
int to_int(const char *s, int *out);

#endif
