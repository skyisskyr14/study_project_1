#ifndef VOTE_H
#define VOTE_H

#include <mysql/mysql.h>
#include "models.h"

/* 执行投票流程（含权限和防刷校验） */
int vote_cast(MYSQL *conn, const User *current_user);

/* 管理员撤销某用户投票 */
int vote_revoke(MYSQL *conn, const char *operator_name);

#endif
