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
 * 使用 UDP 实现服务器端，具体流程如下所示：
 *
 * 创建 socket 连接：sockfd = socket(AF_INET, SOCK_DGRAM, 0);
 * bind 监听接口：bind(sockfd, &serv_addr, sizeof(serv_addr);
 *
 * // 由于 UDP 不需要进行三次握手，建立连接，所以省略了 listen 和 accept 方法，可以直接调用 recvfrom 接收客户端发送过来的信息
 * while(1) {
 *      // recvfrom 可以接受客户端发送过来的信息，同时获取客户端地址
 *      recvfrom(sockfd, buf, sizeof(buf), 0, &cli_addr, &cli_addr_len);
 *      小写 --> 大写
 *      // 将处理之后的信息发送给客户端
 *      sendto(sockfd, buf, strlen(buf), 0, &cli_addr, cli_addr_len);
 * }
 *
 */

int main() {

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len;
    int sockfd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int i, n;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    printf("Accepting connections....");

    while (1) {
        cli_addr_len = sizeof(cli_addr);
        // recvfrom 可以接受客户端发送过来的信息，同时获取客户端地址
        ssize_t ret = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *) &cli_addr, &cli_addr_len);

        if (ret == -1){
            perror("recvfrom error...");
            exit(1);
        }

        printf("received from [%s] at port [%d]",
               inet_ntop(AF_INET, &cli_addr.sin_addr.s_addr, str, sizeof(str)),
               ntohs(cli_addr.sin_port));

        for (int j = 0; j < ret; ++j) {
            buf[j] = toupper(buf[j]);
        }

        // 将处理之后的信息发送给客户端
        ret = sendto(sockfd, buf, ret, 0, (struct sockaddr *) &cli_addr, cli_addr_len);

        if (ret < 0) {
            perror("send msg error");
            exit(1);
        }
    }

    close(sockfd);

    return 0;
}