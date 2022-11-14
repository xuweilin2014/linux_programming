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
    fd_set listen_set, all_set;
    FD_ZERO(&all_set);
    FD_SET(listenfd, &all_set);

    while (1) {
        listen_set = all_set;
        int ret = select(maxfd + 1, &listen_set, NULL, NULL, NULL);

        if (ret < 0) {
            perr_exit("select error");
        } else {
            if (FD_ISSET(listenfd, &listen_set)) {
                cli_addr_len = sizeof(cli_addr);
                connfd = Accept(listenfd, (struct sockaddr *) &cli_addr, &cli_addr_len);

                FD_SET(connfd, &all_set);

                if (maxfd < connfd) {
                    maxfd = connfd;
                }

                if (--ret == 0) {
                    continue;
                }
            }

            for (int i = listenfd + 1; i <= maxfd; ++i) {
                if (FD_ISSET(i, &listen_set)) {
                    if ((n = Read(i, buf, sizeof(buf))) == 0) {
                        Close(i);
                        FD_CLR(i, &all_set);
                    } else if (n > 0) {
                        for (int j = 0; j < n; ++j) {
                            buf[j] = toupper(buf[j]);
                        }

                        write(STDOUT_FILENO, buf, n);
                        write(i, buf, n);
                    }
                }
            }
        }
    }


    return 0;
}