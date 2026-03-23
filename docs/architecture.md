# 系统架构设计

## 1. 模块划分
- `user`：用户注册、登录、密码修改、用户状态管理。
- `vote`：投票流程、防刷票、投票撤销。
- `candidate`：候选人增删改查、排序展示、投票时间配置。
- `stats`：统计报表输出。
- `log`：操作日志记录与查询。
- `db`：数据库连接与基础执行接口。
- `ui`：终端菜单交互与模块调度。

## 2. 数据结构
- `User`：用户名、密码哈希、角色、是否已投票。
- `Candidate`：名称、简介、分类、得票数。
- `VoteRecord`：投票人 ID、候选人 ID、时间、IP。
- `LogRecord`：操作人、操作类型、操作结果、时间。

## 3. 接口规范
- 模块暴露头文件在 `include/`。
- 每个模块提供最小可用的函数集合，如：
  - `user_register/user_login`
  - `vote_cast/vote_revoke`
  - `candidate_add/candidate_list`
  - `stats_show_report`

## 4. 数据持久化方案
- 统一采用 MySQL 表结构：`users/candidates/vote_records/logs/system_config`。
- 通过 SQL 文件提供初始化和种子数据。

## 5. 终端交互
- 访客菜单：注册、登录、退出。
- 登录后根据角色跳转管理员菜单或普通用户菜单。
