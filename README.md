# 纯 C 终端投票系统（MySQL）

本项目是一个按模块拆分的教学示例，满足以下目标：
- 纯 C 语言
- 纯终端界面
- MySQL 持久化
- 按“用户/投票/候选人/统计日志/交互部署”分模块开发

## 快速开始（仅服务端/客户端模式）
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
3. 运行服务端/客户端
   - `voting_server`：TCP 服务端（默认监听 `9090`，每个连接独立会话）
   - `voting_client`：TCP 客户端（默认连接 `127.0.0.1:9090`）

### 启动服务端
```bash
./voting_server
```

> 服务端会把原来的 `run_app()` 终端交互流程直接跑在客户端连接上，
> 也就是：客户端连上后，看到的就是完整“注册/登录/投票/管理”菜单，而不是演示框架命令。

### 连接服务端
```bash
# 本机
./voting_client

# 或指定服务端 IP
./voting_client 192.168.1.100
```

### 使用说明
- 客户端连接成功后直接按原系统菜单操作即可（输入数字、用户名、密码等）。
- 退出方式与单机版一致：在菜单里选择 `0` 退出登录/退出系统。

## 编译模式说明
- 检测到 MySQL/MariaDB 开发库（如 `mysql/mysql.h`、`mariadb/mysql.h` 或 `mysql.h` + client lib）时：使用真实 MySQL 模式。
- 未检测到时：自动回退到 stub 模式，程序可编译但会在启动时提示数据库不可用。
- Makefile 会按以下顺序自动探测依赖：
  1. `mysql_config`
  2. `mariadb_config`
  3. `pkg-config libmysqlclient`
  4. `pkg-config mariadb`
  5. `pkg-config mariadbclient`
  6. 系统默认路径（如 `/usr/include/mysql`、`/usr/include/mariadb`、`/usr/include/mariadb/server`）

### 常见问题（已安装却仍显示 stub）
```bash
# 1) 看是否能找到探测工具
which mysql_config
which mariadb_config

# 2) 看 pkg-config 是否有条目
pkg-config --cflags libmysqlclient
pkg-config --cflags mariadb

# 3) 确认头文件存在
ls /usr/include/mysql/mysql.h
ls /usr/include/mariadb/mysql.h
```

若上述任一项可用，重新 `make clean && make` 后应进入真实数据库模式。

> 注意：如果目录里已经有旧的 `voting_server` / `voting_client` 可执行文件，
> 请务必先执行 `make clean` 再 `make`，并重启服务端，避免继续运行旧的 stub 版本。
>
> 新版服务端启动时会打印 `服务端数据库编译模式: REAL/STUB`，
> 客户端连接后也会看到 `[SERVER] 数据库编译模式: REAL/STUB`，
> 可直接确认当前运行的服务端是不是你刚编译出来的版本。

更多细节见 `docs/` 目录。
