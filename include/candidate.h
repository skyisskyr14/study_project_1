#ifndef CANDIDATE_H
#define CANDIDATE_H

#include <mysql/mysql.h>

/* 新增候选人 */
int candidate_add(MYSQL *conn, const char *operator_name);

/* 删除候选人 */
int candidate_delete(MYSQL *conn, const char *operator_name);

/* 修改候选人 */
int candidate_update(MYSQL *conn, const char *operator_name);

/* 展示候选人列表，可按 name / vote_count 排序 */
int candidate_list(MYSQL *conn, const char *order_field);

/* 设置投票时间窗口 */
int config_set_voting_window(MYSQL *conn, const char *operator_name);

#endif
