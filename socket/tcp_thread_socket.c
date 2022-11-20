//
// Created by xuweilin on 22-11-11.
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

#define MAXLINE 8192
#define SERV_PORT 8000

/**
 * 使用多线程来实现服务器
 */

struct s_info {
    struct sockaddr_in cli_addr;
    int cli_fd;
};

void *do_work(void *argc) {

    struct s_info* si = (struct s_info*) argc;
    int cli_fd = si->cli_fd;
    char buf[MAXLINE], str[INET_ADDRSTRLEN];
    int n, i;

    while (1) {
        // 读客户端
        n = Read(cli_fd, buf, sizeof(buf));
        if (n == 0) {
            printf("the client %d closed...\n", si->cli_fd);
            break;
        }

        printf("received from %s at port %d\n",
               inet_ntop(AF_INET, &(si->cli_addr.sin_addr.s_addr), str, sizeof(str)),
               ntohs(si->cli_addr.sin_port));

        write(STDOUT_FILENO, buf, n);
        for (int j = 0; j < n; ++j) {
            buf[j] = toupper(buf[j]);
        }
        write(cli_fd, buf, n);
    }

    close(cli_fd);
    return (void *) 0;
}

int main() {

    struct sockaddr_in serv_addr, client_addr;
    socklen_t cli_addr_len;
    int serv_fd, cli_fd;
    struct s_info ts[256];
    pthread_t tid;
    int i = 0;

    serv_fd = Socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(serv_fd, &serv_addr, sizeof(serv_addr));
    Listen(serv_fd, 128);

    printf("Accepting client connect ...\n");

    while (1) {
        cli_addr_len = sizeof(client_addr);
        // 阻塞监听客户端连接请求
        cli_fd = Accept(serv_fd, (struct sockaddr *) &client_addr, &cli_addr_len);

        ts[i].cli_fd = cli_fd;
        ts[i].cli_addr = client_addr;

        pthread_create(&tid, NULL, do_work, (void *)&ts[i]);
        // 子线程分离，防止僵尸线程产生
        pthread_detach(tid);
        i++;
    }

    return 0;
}