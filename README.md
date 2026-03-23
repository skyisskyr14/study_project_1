# 纯 C 终端投票系统（MySQL）

本项目是一个按模块拆分的教学示例，满足以下目标：
- 纯 C 语言
- 纯终端界面
- MySQL 持久化
- 按“用户/投票/候选人/统计日志/交互部署”分模块开发

## 快速开始
1. 初始化数据库
   ```bash
   # Windows 请先执行: chcp 65001
   mysql -u root -p --default-character-set=utf8mb4 < sql/schema.sql
   mysql -u root -p --default-character-set=utf8mb4 < sql/seed.sql
   ```
2. 编译
   ```bash
   make
   ```
3. 运行
   ```bash
   ./voting_system
   ```

更多细节见 `docs/` 目录。
