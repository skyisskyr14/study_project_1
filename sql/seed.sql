USE voting_system;

-- 默认管理员，密码为 admin123（由当前代码的 hash_password 算法计算结果）
INSERT INTO users(username, password_hash, user_type, has_voted)
VALUES ('admin', '000000000000000000000000000000000000000000000000000006531ca97027', 'admin', 0)
ON DUPLICATE KEY UPDATE username = VALUES(username);

INSERT INTO candidates(name, category, bio, vote_count)
VALUES
('张三', '学生代表', '关注教学质量与实验资源', 0),
('李四', '学生代表', '关注社团与校园活动体验', 0)
ON DUPLICATE KEY UPDATE name = VALUES(name);
