#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 9090
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    const char *server_ip = (argc >= 2) ? argv[1] : "127.0.0.1";
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "socket 创建失败: %s\n", strerror(errno));
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "无效服务端 IP: %s\n", server_ip);
        close(sockfd);
        return 1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "连接失败: %s\n", strerror(errno));
        close(sockfd);
        return 1;
    }

    printf("已连接到服务端 %s:%d\n", server_ip, SERVER_PORT);
    fflush(stdout);

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sockfd, &readfds);

        int max_fd = (sockfd > STDIN_FILENO) ? sockfd : STDIN_FILENO;
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            if (errno == EINTR) continue;
            fprintf(stderr, "select 失败: %s\n", strerror(errno));
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            char inbuf[BUFFER_SIZE];
            ssize_t n = read(STDIN_FILENO, inbuf, sizeof(inbuf));
            if (n <= 0) {
                shutdown(sockfd, SHUT_WR);
            } else {
                ssize_t sent = 0;
                while (sent < n) {
                    ssize_t m = send(sockfd, inbuf + sent, (size_t)(n - sent), 0);
                    if (m <= 0) {
                        fprintf(stderr, "发送失败: %s\n", strerror(errno));
                        close(sockfd);
                        return 1;
                    }
                    sent += m;
                }
            }
        }

        if (FD_ISSET(sockfd, &readfds)) {
            char outbuf[BUFFER_SIZE];
            ssize_t n = recv(sockfd, outbuf, sizeof(outbuf), 0);
            if (n <= 0) {
                break;
            }
            ssize_t written = 0;
            while (written < n) {
                ssize_t m = write(STDOUT_FILENO, outbuf + written, (size_t)(n - written));
                if (m <= 0) {
                    fprintf(stderr, "输出失败: %s\n", strerror(errno));
                    close(sockfd);
                    return 1;
                }
                written += m;
            }
        }
    }

    close(sockfd);
    printf("\n连接已关闭。\n");
    return 0;
}
