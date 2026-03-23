#ifndef UI_H
#define UI_H

#include "models.h"

/* 启动交互主循环 */
void run_app(void);

/* 管理员菜单 */
void run_admin_menu(void *conn, const User *current_user);

/* 普通用户菜单 */
void run_voter_menu(void *conn, const User *current_user);

#endif
