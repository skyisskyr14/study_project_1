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
3. 运行（单机终端版）
   ```bash
   ./voting_system
   ```

## 服务端/客户端模式（基础框架）
- 编译后会额外生成：
  - `voting_server`：TCP 服务端（默认监听 `9090`）
  - `voting_client`：TCP 客户端（默认连接 `127.0.0.1:9090`）

### 启动服务端
```bash
./voting_server
```

### 连接服务端
```bash
# 本机
./voting_client

# 或指定服务端 IP
./voting_client 192.168.1.100
```

### 当前支持命令
- `HELP`
- `PING`
- `QUIT`

## 编译模式说明
- 检测到 MySQL 开发库（`mysql/mysql.h` + client lib）时：使用真实 MySQL 模式。
- 未检测到时：自动回退到 stub 模式，程序可编译但会在启动时提示数据库不可用。

更多细节见 `docs/` 目录。
