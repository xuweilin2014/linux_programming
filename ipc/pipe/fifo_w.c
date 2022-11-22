//
// Created by xuweilin on 22-11-4.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

/**
 * fifo 命名管道可以用于无血缘进程之间的通信，下面的程序向命名管道中写入数据
 */

// 不停的往命名管道中写入数据
int main() {
    int fd, i;
    char buf[4096];

    fd = open("../ipc/testfifo", O_WRONLY | O_CREAT);
    if (fd < 0) {
        perror("open error");
        exit(1);
    }

    i = 0;
    while (1) {
        sprintf(buf, "hello world%d\n", i++);
        write(fd, buf, strlen(buf));
        sleep(1);
    }

    close(fd);

    return 0;
}