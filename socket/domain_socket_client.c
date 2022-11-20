//
// Created by xuweilin on 22-11-20.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>

#define SERV_ADDR "serv.socket"
#define CLI_ADDR "cli.socket"

int main() {

    int cfd, len;
    struct sockaddr_un serv_addr, cli_addr;
    char buf[BUFSIZ];

    cfd = socket(AF_UNIX, SOCK_STREAM, 0);

    /*
     * 客户端不能依赖 “隐式绑定”。并且应该在通信建立过程中，创建且初始化 2 个地址结构：
     *
     * 1.client_addr --> bind()
     * 2.server_addr --> connect()
     *
     */

    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sun_family = AF_UNIX;
    strcpy(cli_addr.sun_path, CLI_ADDR);
    /* 计算客户端地址结构的有效长度 */
    len = offsetof(struct sockaddr_un, sun_path) + strlen(cli_addr.sun_path);
    /* 这里需要 unlink 是因为有可能 CLI_ADDR 已经存在，会出现 address already in use 错误，所以需要先删除 */
    unlink(CLI_ADDR);
    /* 客户端也需要 bind，不能依赖自动绑定 */
    bind(cfd, (struct sockaddr *) &cli_addr, len);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SERV_ADDR);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(serv_addr.sun_path);
    // 连接到服务端进程的本地套接字，len 值不为 sizeof(serv_addr)，而是 2 (sun_path) + strlen(sun_path)，也就是实际长度
    // 另外，客户端对于自己的 cli_addr 需要绑定，而对于服务端的 serv_addr 则不需要绑定，因为这是需要客户端进行通信的一方，按理来说，
    // 服务端的地址结构 serv_addr 已经存在是非常正常的
    connect(cfd, (struct sockaddr *) &serv_addr, len);

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        write(cfd, buf, strlen(buf));
        len = read(cfd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, len);
    }

    close(cfd);

    return 0;
}