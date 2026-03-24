-- 投票系统数据库初始化脚本
CREATE DATABASE IF NOT EXISTS voting_system DEFAULT CHARSET utf8mb4;
USE voting_system;

CREATE TABLE IF NOT EXISTS users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(31) NOT NULL UNIQUE,
    password_hash CHAR(64) NOT NULL,
    user_type ENUM('admin', 'voter') NOT NULL DEFAULT 'voter',
    has_voted TINYINT NOT NULL DEFAULT 0,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS candidates (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(63) NOT NULL,
    category VARCHAR(63) NOT NULL,
    bio VARCHAR(255) NOT NULL,
    vote_count INT NOT NULL DEFAULT 0,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS vote_records (
    id INT PRIMARY KEY AUTO_INCREMENT,
    user_id INT NOT NULL,
    candidate_id INT NOT NULL,
    vote_category VARCHAR(63) NOT NULL,
    ip_addr VARCHAR(45) NOT NULL,
    voted_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    UNIQUE KEY uk_vote_user_category (user_id, vote_category),
    UNIQUE KEY uk_vote_ip_category (ip_addr, vote_category),
    CONSTRAINT fk_vote_user FOREIGN KEY (user_id) REFERENCES users(id),
    CONSTRAINT fk_vote_candidate FOREIGN KEY (candidate_id) REFERENCES candidates(id)
);

CREATE TABLE IF NOT EXISTS logs (
    id INT PRIMARY KEY AUTO_INCREMENT,
    operator_name VARCHAR(31) NOT NULL,
    action_type VARCHAR(31) NOT NULL,
    action_result VARCHAR(63) NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS system_config (
    config_key VARCHAR(64) PRIMARY KEY,
    config_value VARCHAR(64) NOT NULL
);

INSERT INTO system_config(config_key, config_value)
VALUES
('vote_start', '2026-01-01 00:00:00'),
('vote_end', '2026-12-31 23:59:59')
ON DUPLICATE KEY UPDATE config_value = VALUES(config_value);
