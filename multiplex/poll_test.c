//
// Created by xuweilin on 22-11-14.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>
#include "wrap_socket.h"
#include <poll.h>

#define MAXLINE 80
#define SERV_PORT 6666
#define OPEN_MAX 1024

/**
 * 使用 poll 函数来使服务器连接/处理多个客户端请求
 * @return
 */

int main() {

    int i, j, maxi, listenfd, connfd, sockfd;
    int nready;
    ssize_t n;
    char buf[MAXLINE], str[INET_ADDRSTRLEN];
    socklen_t cli_addr_len;
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in cli_addr, serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    // 实现端口重用
    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    Listen(listenfd, 128);

    client[0].fd = listenfd;
    client[0].events = POLLIN;

    for (int k = 1; k < OPEN_MAX; ++k) {
        client[k].fd = -1;
    }

    maxi = 0;

    while (1) {
        nready = poll(client, maxi + 1, -1);

        if (nready > 0) {
            if (client[0].revents & POLLIN) {
                cli_addr_len = sizeof(cli_addr);
                connfd = Accept(listenfd, (struct sockaddr *) &cli_addr, &cli_addr_len);

                printf("received from %s at PORT %d\n",
                       inet_ntop(AF_INET, &(cli_addr.sin_addr.s_addr), str, sizeof(str)),
                       ntohs(cli_addr.sin_port));

                for (i = 1; i < OPEN_MAX; ++i) {
                    if (client[i].fd < 0) {
                        client[i].fd = connfd;
                        client[i].events = POLLIN;
                        break;
                    }
                }

                if (i == OPEN_MAX) {
                    perr_exit("too many clients....");
                }

                if (i > maxi) {
                    maxi = i;
                }

                if (--nready == 0)
                    continue;
            }

            for (i = 1; i <= maxi; ++i) {
                if ((sockfd = client[i].fd) < 0)
                    continue;

                if (client[i].revents & POLLIN) {
                    if ((n = Read(sockfd, buf, MAXLINE)) < 0) {
                        perr_exit("read error");
                    } else if (n == 0) {
                        printf("client[%d] close connection..", i);
                        Close(sockfd);
                        client[i].fd = -1;
                    } else {
                        for (int k = 0; k < n; ++k) {
                            buf[k] = toupper(buf[k]);
                        }
                        Write(STDOUT_FILENO, buf, n);
                        Write(sockfd, buf, n);
                    }

                    if (--nready == 0)
                        break;
                }
            }
        }
    }

    return 0;
}