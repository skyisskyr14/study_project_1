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

#define SERVER_PORT 9090
#define BUFFER_SIZE 1024

static int send_text(int fd, const char *text) {
    size_t len = strlen(text);
    return send(fd, text, len, 0) == (ssize_t)len;
}

static void handle_client(int client_fd, const char *client_ip) {
    char buffer[BUFFER_SIZE];
    ssize_t n;

    printf("[server] client connected: %s\n", client_ip);
    send_text(client_fd,
              "欢迎连接投票服务端。\n"
              "可用命令: HELP / PING / QUIT\n");

    while ((n = recv(client_fd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[n] = '\0';

        char *newline = strchr(buffer, '\n');
        if (newline) *newline = '\0';

        if (strcmp(buffer, "PING") == 0) {
            send_text(client_fd, "PONG\n");
        } else if (strcmp(buffer, "HELP") == 0) {
            send_text(client_fd,
                      "当前为基础 C/S 连接框架。\n"
                      "后续可在此协议上扩展登录、拉取候选、投票等命令。\n");
        } else if (strcmp(buffer, "QUIT") == 0) {
            send_text(client_fd, "BYE\n");
            break;
        } else if (strlen(buffer) == 0) {
            continue;
        } else {
            send_text(client_fd, "未知命令，输入 HELP 查看可用命令。\n");
        }
    }

    if (n < 0) {
        fprintf(stderr, "[server] recv error: %s\n", strerror(errno));
    }
    close(client_fd);
    printf("[server] client disconnected: %s\n", client_ip);
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

    if (listen(server_fd, 8) < 0) {
        fprintf(stderr, "listen 失败: %s\n", strerror(errno));
        close(server_fd);
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);
    printf("投票服务端已启动: 0.0.0.0:%d\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            fprintf(stderr, "accept 失败: %s\n", strerror(errno));
            continue;
        }

        char client_ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        handle_client(client_fd, client_ip);
    }

    close(server_fd);
    return 0;
}
