# 部署与运行说明

## 1. 环境要求
- Linux / macOS / Windows
- GCC (支持 C11)
- MySQL Server 8+
- MySQL C Client 开发库（`mysql_config` 可用）

## 2. 初始化数据库
### Linux / macOS
```bash
mysql -u root -p --default-character-set=utf8mb4 < sql/schema.sql
mysql -u root -p --default-character-set=utf8mb4 < sql/seed.sql
```

### Windows CMD / PowerShell
先切换终端编码到 UTF-8，再导入：
```bat
chcp 65001
mysql -u root -p --default-character-set=utf8mb4 < sql/schema.sql
mysql -u root -p --default-character-set=utf8mb4 < sql/seed.sql
```

## 3. 编译
```bash
make
```

## 4. 运行
```bash
./voting_system
```

## 5. 数据库配置
默认配置在 `src/ui.c`：
- host: `127.0.0.1`
- port: `3306`
- user: `root`
- password: `123456`
- database: `voting_system`

请按实际环境修改后再运行。
