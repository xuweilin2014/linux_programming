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
#include <sys/epoll.h>
#include "wrap_socket.h"

#define MAXLINE 8192
#define SERV_PORT 8000
#define OPEN_MAX 1024

/**
 * 使用 epoll 函数实现服务器连接/处理多个客户端请求
 * @return
 */

int main() {

    int listenfd, connfd, sockfd;
    int epfd;
    int ret, nready, n;
    // tep: epoll_ctl 参数  ep[] : epoll_wait 参数
    struct epoll_event tep, ep[OPEN_MAX];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len;
    char str[INET_ADDRSTRLEN], buf[MAXLINE];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    Bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    Listen(listenfd, 128);

    // 创建 epoll 模型, efd 指向红黑树根节点
    epfd = epoll_create(OPEN_MAX);
    if (epfd == -1) {
        perr_exit("epoll create error");
    }

    tep.data.fd = listenfd;
    // 指定 lfd 的监听时间为"读"
    tep.events = EPOLLIN;
    // 将 lfd 及对应的结构体设置到树上，efd 可找到该树
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &tep);
    if (ret < 0) {
        perr_exit("epoll ctl error");
    }

    while (1) {
        /* epoll为 server 阻塞监听事件, ep 为 struct epoll_event 类型数组, OPEN_MAX 为数组容量, -1 表永久阻塞 */
        nready = epoll_wait(epfd, (struct epoll_event *) &ep, OPEN_MAX, -1);

        if (nready < 0) {
            perr_exit("epoll wait error");
        } else if (nready > 0) {
            for (int i = 0; i < nready; ++i) {
                if (!(ep[i].events & EPOLLIN))
                    continue;

                if (ep[i].data.fd == listenfd) {
                    cli_addr_len = sizeof(cli_addr);
                    connfd = Accept(listenfd, (struct sockaddr *) &cli_addr, &cli_addr_len);

                    printf("received from %s at PORT %d\n",
                           inet_ntop(AF_INET, &(cli_addr.sin_addr.s_addr), str, sizeof(str)),
                           ntohs(cli_addr.sin_port));

                    // 将读取到的客户端连接添加到红黑树上进行监听
                    tep.data.fd = connfd;
                    tep.events = EPOLLIN;

                    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &tep);
                    if (ret < 0) {
                        perr_exit("epoll ctl error");
                    }
                } else {
                    sockfd = ep[i].data.fd;
                    n = Read(sockfd, buf, sizeof(buf));

                    if (n <= 0) {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
                        Close(sockfd);

                        // 说明客户端连接关闭了连接
                        if (n == 0)
                            printf("client[%d] closed connection.", sockfd);

                    } else {
                        for (int j = 0; j < n; ++j) {
                            buf[j] = toupper(buf[j]);
                        }

                        write(sockfd, buf, n);
                        write(STDOUT_FILENO, buf, n);
                    }
                }
            }
        }
    }

    close(listenfd);
    close(epfd);

    return 0;
}