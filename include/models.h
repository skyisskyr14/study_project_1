#ifndef MODELS_H
#define MODELS_H

#include <stddef.h>

#define USERNAME_MAX_LEN 32
#define PASSWORD_HASH_LEN 65
#define USER_TYPE_MAX_LEN 16
#define IP_ADDR_MAX_LEN 46
#define CANDIDATE_NAME_MAX_LEN 64
#define CANDIDATE_CATEGORY_MAX_LEN 64
#define CANDIDATE_BIO_MAX_LEN 256
#define LOG_TYPE_MAX_LEN 32
#define LOG_RESULT_MAX_LEN 64

/* 用户结构体：用于用户模块数据交互 */
typedef struct {
    int id;
    char username[USERNAME_MAX_LEN];
    char password_hash[PASSWORD_HASH_LEN];
    char user_type[USER_TYPE_MAX_LEN]; /* admin / voter */
    int has_voted;
    char created_at[20];
} User;

/* 候选人结构体：用于候选人管理与展示 */
typedef struct {
    int id;
    char name[CANDIDATE_NAME_MAX_LEN];
    char category[CANDIDATE_CATEGORY_MAX_LEN];
    char bio[CANDIDATE_BIO_MAX_LEN];
    int vote_count;
} Candidate;

/* 投票记录结构体：用于防刷票与投票追踪 */
typedef struct {
    int id;
    int user_id;
    int candidate_id;
    char vote_category[CANDIDATE_CATEGORY_MAX_LEN];
    char ip_addr[IP_ADDR_MAX_LEN];
    char voted_at[20];
} VoteRecord;

/* 日志结构体：用于系统行为记录 */
typedef struct {
    int id;
    char operator_name[USERNAME_MAX_LEN];
    char action_type[LOG_TYPE_MAX_LEN];
    char action_result[LOG_RESULT_MAX_LEN];
    char created_at[20];
} LogRecord;

#endif
