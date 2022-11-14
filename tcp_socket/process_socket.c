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

#define SERV_PORT 9999

/**
 * 实现多进程服务器
 * @param sig
 */

void recover_process(int sig) {

    while ((waitpid(0, NULL, WNOHANG)) > 0);

}

int main() {

    int lfd, cfd;
    pid_t pid;
    struct sockaddr_in srv_addr, clt_addr;
    socklen_t clt_addr_len = sizeof(clt_addr);
    char buf[BUFSIZ];
    ssize_t ret;

    memset(&srv_addr, 0, sizeof(srv_addr));

    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SERV_PORT);
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = socket(AF_INET, SOCK_STREAM, 0);

    // 设置端口复用，因此当先关闭掉服务器且服务器处于 TIME-WAIT 阶段时，不会出现 bind error addr already in use
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEPORT, (void *)&opt, sizeof(opt));

    Bind(lfd, (struct sockaddr *) &srv_addr, sizeof(srv_addr));
    Listen(lfd, 128);

    while (1) {
        // 主进程监听到有客户端连接之后，就会创建一个子进程来处理这个连接
        cfd = Accept(lfd, (struct sockaddr *) &clt_addr, &clt_addr_len);

        pid = fork();
        if (pid < 0) {
            perror("fork error");
        } else if (pid == 0) {
            close(lfd);
            break;
        } else {
            // 主进程通过信号 SIGCHLD 来实现回收子进程
            struct sigaction act;

            act.sa_flags = 0;
            sigemptyset(&act.sa_mask);
            act.sa_handler = recover_process;

            sigaction(SIGCHLD, &act, NULL);
            if (ret != 0) {
                perr_exit("sigaction error");
            }
            close(cfd);

            continue;
        }
    }

    if (pid == 0) {
        printf("child process\n");
        for(;;) {
            ret = Read(cfd, buf, sizeof(buf));

            printf("ret: %d\n", ret);
            if (ret == 0) {
                close(cfd);
                exit(2);
            }

            for (int j = 0; j < ret; ++j) {
                buf[j] = toupper(buf[j]);
            }

            write(cfd, buf, ret);
            write(STDOUT_FILENO, buf, ret);
        }
    }

    return 0;
}