#ifndef LOG_H
#define LOG_H

#include <mysql/mysql.h>

/* 记录系统日志 */
int log_action(MYSQL *conn, const char *operator_name, const char *action_type, const char *action_result);

/* 按操作类型筛选展示日志，传空字符串表示不过滤 */
int log_query(MYSQL *conn, const char *action_type_filter);

#endif
