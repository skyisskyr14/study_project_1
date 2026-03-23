#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "mysql_compat.h"
#include "ui.h"

#define SERVER_PORT 9090

static void run_session_on_socket(int client_fd) {
    if (dup2(client_fd, STDIN_FILENO) < 0 ||
        dup2(client_fd, STDOUT_FILENO) < 0 ||
        dup2(client_fd, STDERR_FILENO) < 0) {
        fprintf(stderr, "dup2 失败: %s\n", strerror(errno));
        return;
    }
    close(client_fd);

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#ifdef MYSQL_STUB
    printf("[SERVER] 数据库编译模式: STUB\n");
#else
    printf("[SERVER] 数据库编译模式: REAL\n");
#endif

    run_app();
}

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        fprintf(stderr, "socket 创建失败: %s\n", strerror(errno));
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "bind 失败: %s\n", strerror(errno));
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 16) < 0) {
        fprintf(stderr, "listen 失败: %s\n", strerror(errno));
        close(server_fd);
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
#ifdef MYSQL_STUB
    printf("服务端数据库编译模式: STUB\n");
#else
    printf("服务端数据库编译模式: REAL\n");
#endif
    printf("投票服务端已启动: 0.0.0.0:%d\n", SERVER_PORT);
    fflush(stdout);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            fprintf(stderr, "accept 失败: %s\n", strerror(errno));
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "fork 失败: %s\n", strerror(errno));
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            close(server_fd);
            run_session_on_socket(client_fd);
            return 0;
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
