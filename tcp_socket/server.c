//
// Created by xuweilin on 22-10-22.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>

#define SERV_PORT 9523

void sys_err(const char* str) {
    perror(str);
    exit(1);
}

/**
 * TCP 通信流程分析：
 *
 *      server:
 *          1.socket 创建 socket
 *          2.bind   绑定服务器监听的地址结构
 *          3.listen 设置监听上限
 *          4.accept 阻塞监听客户端连接
 *          5.read(fd) 读取 socket 获取客户端数据
 *          6.小--大写 toupper()
 *          7.write(fd)
 *          8.close()
 * @return
 */

int main() {
    int lfd = 0, cfd = 0;

    struct sockaddr_in serv_addr, clit_addr;
    socklen_t clit_addr_len;
    char buf[BUFSIZ], client_ip[1024];

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        sys_err("socket error");
    }

    bind(lfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(lfd, 256);

    clit_addr_len = sizeof(clit_addr);

    cfd = accept(lfd, (struct sockaddr *) &clit_addr, &clit_addr_len);

    printf("client ip:%s, port:%d\n",
           inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, client_ip, sizeof(clit_addr)),
           ntohs(clit_addr.sin_port));

    if (cfd == -1) {
        sys_err("accept error");
    }

    while (1) {
        int ret = read(cfd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);

        for(int i = 0; i < ret; i++) {
            buf[i] = toupper(buf[i]);
        }

        write(cfd, buf, ret);
    }

    close(lfd);
    close(cfd);

    return 0;

}