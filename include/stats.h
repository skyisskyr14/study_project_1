#ifndef STATS_H
#define STATS_H

#include <mysql/mysql.h>

/* 展示统计报表 */
int stats_show_report(MYSQL *conn);

#endif
