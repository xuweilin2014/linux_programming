//
// Created by xuweilin on 22-11-13.
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

#define SERV_PORT 6666
#define BUFSIZ 1028

/**
 * 使用 select 函数来使服务器连接/处理多个客户端请求
 * @return
 */

int main() {

    int listenfd, connfd;
    int maxfd, n;
    char buf[BUFSIZ];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    Listen(listenfd, 128);

    maxfd = listenfd;
    /* listen_set 读事件文件描述符集合 all_set 用来暂存 */
    fd_set listen_set, all_set;
    FD_ZERO(&all_set);
    /* 构造 select 监控文件描述符集 */
    FD_SET(listenfd, &all_set);

    while (1) {
        listen_set = all_set;
        int ret = select(maxfd + 1, &listen_set, NULL, NULL, NULL);

        if (ret < 0) {
            perr_exit("select error");
        } else {
            /* 说明有新的客户端链接请求 */
            if (FD_ISSET(listenfd, &listen_set)) {
                cli_addr_len = sizeof(cli_addr);
                // Accept 函数不会阻塞
                connfd = Accept(listenfd, (struct sockaddr *) &cli_addr, &cli_addr_len);

                FD_SET(connfd, &all_set);

                if (maxfd < connfd) {
                    maxfd = connfd;
                }

                /* 只有 listenfd 有事件, 后续的 for 不需执行 */
                if (--ret == 0) {
                    continue;
                }
            }

            /* 检测哪个 client 有数据就绪 */
            for (int i = listenfd + 1; i <= maxfd; ++i) {

                if (FD_ISSET(i, &listen_set)) {
                    // read 函数返回 0，对于网络套接字来说意味着对端已经关闭连接
                    if ((n = Read(i, buf, sizeof(buf))) == 0) {
                        Close(i);
                        // 将文件描述符从 all_set 中移除
                        FD_CLR(i, &all_set);
                    } else if (n > 0) {
                        for (int j = 0; j < n; ++j) {
                            buf[j] = toupper(buf[j]);
                        }

                        // 将 buf 中的数据写出到 shell 上
                        write(STDOUT_FILENO, buf, n);
                        // 将 buf 中的数据写回到客户端
                        write(i, buf, n);
                    }

                    if (--ret == 0) {
                        continue;
                    }
                }
            }
        }
    }


    return 0;
}