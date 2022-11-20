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

int main() {

    int lfd, cfd, len, size, i;
    struct sockaddr_un serv_addr, cli_addr;
    char buf[BUFSIZ];
    socklen_t cli_len;

    lfd = socket(AF_UNIX, SOCK_STREAM, 0);

    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, SERV_ADDR);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(serv_addr.sun_path);

    /* 确保bind之前 serv.socket 文件不存在,bind会创建该文件 */
    unlink(SERV_ADDR);
    /* 参3 不能是 sizeof(serv_addr) */
    bind(lfd, (struct sockaddr *) &serv_addr, len);
    listen(lfd, 128);

    printf("Accept...\n");

    cli_len = sizeof(cli_addr);
    cfd = accept(lfd, (struct sockaddr *) &cli_addr, &cli_len);

    /* 得到文件名称的长度 */
    len = cli_len - offsetof(struct sockaddr_un, sun_path);
    cli_addr.sun_path[len] = '\0';
    printf("client bind filename %s\n", cli_addr.sun_path);

    while ((size = read(cfd, buf, sizeof(buf))) > 0) {
        for (int j = 0; j < size; ++j) {
            buf[j] = toupper(buf[j]);
        }
        write(cfd, buf, size);
    }

    close(cfd);
    close(lfd);

    return 0;
}