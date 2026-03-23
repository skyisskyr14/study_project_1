#ifndef USER_H
#define USER_H

#include <mysql/mysql.h>
#include "models.h"

/* 用户注册 */
int user_register(MYSQL *conn);

/* 用户登录，成功后将用户信息写入 out_user */
int user_login(MYSQL *conn, User *out_user);

/* 修改当前登录用户密码 */
int user_change_password(MYSQL *conn, const User *current_user);

/* 根据用户 ID 更新已投票状态 */
int user_mark_voted(MYSQL *conn, int user_id);

/* 展示用户列表（管理员功能） */
int user_list_all(MYSQL *conn);

#endif
