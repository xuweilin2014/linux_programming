//
// Created by xuweilin on 22-10-22.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_PORT 9523

void sys_err(const char* str);

/**
 * TCP 通信流程分析：
 *
 *      client:
 *          1.socket 创建 socket
 *          2.connect 与服务器建立连接
 *          3.write 写数据到 socket
 *          4.read 读取转换后的数据
 *          5.显示读取的结果
 *          6.close
 * @return
 */

int main() {

    int cfd;
    int counter = 10;
    char buf[BUFSIZ];
    unsigned int ip = 0;

    // 服务器地址结构
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &ip);
    serv_addr.sin_addr.s_addr = ip;

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1) {
        sys_err("socket error");
    }

    connect(cfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

    while (counter--) {
        write(cfd, "hello\n", 6);
        int ret = read(cfd, buf, sizeof buf);
        write(STDOUT_FILENO, buf, ret);
    }

    return 0;
}