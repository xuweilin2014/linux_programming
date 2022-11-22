//
// Created by xuweilin on 22-10-31.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * 下面的程序使用 fcntl 来复制文件描述符，实现 dup 函数的功能，返回的文件描述符 newfd 和 newfd2 指向 fd1,
 * 当向 newfd2 写入数据时，数据会被真正写入到文件 lseek.txt 中
 */

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

    printf("ret = %d\n", ret);

    return 0;
}