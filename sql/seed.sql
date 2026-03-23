USE voting_system;
SET NAMES utf8mb4;

-- 默认管理员，密码为 admin123（由当前代码的 make_password_hash 算法计算结果）
INSERT INTO users(username, password_hash, user_type, has_voted)
VALUES ('admin', '000000000000000000000000000000000000000000000000001ae6d5185030e4', 'admin', 0)
ON DUPLICATE KEY UPDATE
    password_hash = VALUES(password_hash),
    user_type = VALUES(user_type),
    has_voted = VALUES(has_voted);

-- 为避免 Windows CMD / PowerShell 编码差异导致导入失败，示例数据使用 ASCII 文本
INSERT INTO candidates(name, category, bio, vote_count)
VALUES
('Alice', 'StudentRep', 'Focus on teaching quality and lab resources.', 0),
('Bob', 'StudentRep', 'Focus on clubs and campus activity experience.', 0)
ON DUPLICATE KEY UPDATE name = VALUES(name), category = VALUES(category), bio = VALUES(bio);
