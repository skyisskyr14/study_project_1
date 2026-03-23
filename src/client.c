#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    printf("输入命令并回车（HELP/PING/QUIT）:\n");

    char send_buf[BUFFER_SIZE];
    char recv_buf[BUFFER_SIZE];

    ssize_t n = recv(sockfd, recv_buf, sizeof(recv_buf) - 1, 0);
    if (n > 0) {
        recv_buf[n] = '\0';
        printf("%s", recv_buf);
    }

    while (fgets(send_buf, sizeof(send_buf), stdin)) {
        if (send(sockfd, send_buf, strlen(send_buf), 0) < 0) {
            fprintf(stderr, "发送失败: %s\n", strerror(errno));
            break;
        }

        n = recv(sockfd, recv_buf, sizeof(recv_buf) - 1, 0);
        if (n <= 0) {
            printf("服务端已断开连接。\n");
            break;
        }
        recv_buf[n] = '\0';
        printf("%s", recv_buf);

        if (strncmp(send_buf, "QUIT", 4) == 0) {
            break;
        }
    }

    close(sockfd);
    return 0;
}
