//
// Created by xuweilin on 22-11-20.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <strings.h>
#include <memory.h>

#define MAXLINE 80
#define SERV_PORT 6666

/**
 * 使用 UDP 实现客户端，具体流程如下：
 *
 * 创建 socket 连接：sockfd = socket(AF_INET, SOCK_DGRAM, 0);
 *
 * // UDP 客户端只需要使用 socket 就可以向对方发送信息，省略了 connect 方法
 * while(fgets() != NULL) {
 *      // 向服务端发送消息
 *      sendto(sockfd, buf, strlen(buf), 0, &serv_addr, serv_addr_len);
 *      // 接收服务端发送过来的消息
 *      recvfrom(sockfd, buf, sizeof(buf), 0, &cli_addr, &cli_addr_len);
 *      write(STDOUT_FILENO, buf, ret);
 * }
 */

int main() {

    struct sockaddr_in serv_addr;
    int sockfd, n;
    char buf[MAXLINE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_family = AF_INET;

    // STDIN_FILENO 数据类型为 int，它是一个系统级的 API，是一个文件句柄，用于 read/write/close/open 等系统调用级别的函数中
    // stdin 数据类型为 FILE*，它是 c 语言提供的标准输入流，用于 fopen/fclose/fread/fwrite 等 c 语言标准函数
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        // 向服务端发送消息
        ssize_t ret = sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
        if (ret < 0) {
            perror("sendto error");
        }

        // 接收服务端发送过来的消息，这里 addr 为 NULL，是因为 recvfrom 会获取到对端的地址，这里获取到的是服务器端的地址，
        // 没有用处，所以将 addr 设置为 NULL
        ret = recvfrom(sockfd, buf, sizeof(buf), 0, NULL, 0);
        if (ret < 0) {
            perror("recvfrom error");
        }

        write(STDOUT_FILENO, buf, ret);
    }

    close(sockfd);

    return 0;
}