//
// Created by xuweilin on 22-10-31.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * 使用 execlp 函数执行 ps aux 命令，并且将命令执行的结果由 STDOUT_FILENO 重定向保存到 ps.txt 文件中
 */

int exec_ps() {

    int fd;

    fd = open("../process/ps.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open ps.txt error");
        exit(1);
    }

    dup2(fd, STDOUT_FILENO);
    execlp("ps", "ps", "aux", NULL);

    return 0;
}