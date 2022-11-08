//
// Created by xuweilin on 22-10-29.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int lseek_test() {
    int fd, n;
    char msg[] = "It's a test for lseek";
    char ch;

    fd = open("../file_io/lseek.txt", O_RDWR | O_CREAT, 0664);
    if (fd < 0) {
        perror("open lseek.txt error");
        exit(1);
    }

    // 使用 fd 对打开的文件进行写操作，之后读位置位于文件结尾处
    // 读写文件共用一个偏移位置
    write(fd, msg, strlen(msg));

    // 修改文件读写指针位置，位于文件头
    lseek(fd, 0, SEEK_SET);

    while ((n = read(fd, &ch, 1))) {
        if (n < 0) {
            perror("read error");
            exit(1);
        }
        write(STDOUT_FILENO, &ch, n);
    }

    close(fd);

    return 0;
}

// 使用 lseek 获取文件大小
int lseek_file_size() {
    int fd = open("../file_io/lseek.txt", O_RDWR);
    if (fd == -1) {
        perror("open error");
        exit(1);
    }

    int length = lseek(fd, 0, SEEK_END);
    printf("file size:%d\n", length);

    return 0;
}