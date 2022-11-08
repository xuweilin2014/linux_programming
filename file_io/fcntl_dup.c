//
// Created by xuweilin on 22-10-31.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int fcntl_dup() {
    int fd1 = open("../file_io/lseek.txt", O_RDWR);
    printf("fd1 = %d\n", fd1);

    // 0 被占用，fcntl 使用文件描述符表中可用的最小文件描述符返回
    int newfd = fcntl(fd1, F_DUPFD, 0);
    printf("newfd = %d\n", newfd);

    // 7，未被占用，返回 >= 7 的文件描述符
    int newfd2 = fcntl(fd1, F_DUPFD, 7);
    printf("newfd2 = %d", newfd2);

    int ret = write(newfd2, "123456", 6);
    return 0;
}