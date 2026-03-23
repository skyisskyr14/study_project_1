#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "common.h"

void read_line(char *buffer, int size) {
    if (fgets(buffer, size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
}

/*
 * 这是一个简化的 djb2 哈希，方便教学演示。
 * 生产环境请替换为带盐的强哈希算法。
 */
void make_password_hash(const char *password, char out_hash[65]) {
    unsigned long hash = 5381;
    int c = 0;
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + (unsigned long)c;
    }
    snprintf(out_hash, 65, "%064lx", hash);
}

int is_valid_username(const char *username) {
    size_t len = strlen(username);
    if (len < 3 || len >= USERNAME_MAX_LEN) {
        return 0;
    }
    for (size_t i = 0; i < len; i++) {
        if (!(isalnum((unsigned char)username[i]) || username[i] == '_')) {
            return 0;
        }
    }
    return 1;
}

int is_valid_password(const char *password) {
    size_t len = strlen(password);
    int has_alpha = 0;
    int has_digit = 0;
    if (len < 6 || len > 32) {
        return 0;
    }
    for (size_t i = 0; i < len; i++) {
        if (isalpha((unsigned char)password[i])) {
            has_alpha = 1;
        }
        if (isdigit((unsigned char)password[i])) {
            has_digit = 1;
        }
    }
    return has_alpha && has_digit;
}

void now_str(char out[20]) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(out, 20, "%Y-%m-%d %H:%M:%S", tm_info);
}

int to_int(const char *s, int *out) {
    int value = 0;
    char extra = '\0';
    if (sscanf(s, "%d%c", &value, &extra) != 1) {
        return 0;
    }
    *out = value;
    return 1;
}
